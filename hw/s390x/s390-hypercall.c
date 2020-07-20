/*
 * Support for QEMU/KVM hypercalls on s390
 *
 * Copyright 2012 IBM Corp.
 * Author(s): Cornelia Huck <cornelia.huck@de.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "hw/boards.h"
#include "hw/s390x/s390-hypercall.h"
#include "hw/s390x/ioinst.h"
#include "hw/s390x/css.h"
#include "virtio-ccw.h"

static int handle_virtio_notify(uint64_t mem)
{
    MachineState *ms = MACHINE(qdev_get_machine());

    if (mem < ms->ram_size) {
        /* Early printk */
        return 0;
    }
    return -EINVAL;
}

static int handle_virtio_ccw_notify(uint64_t subch_id, uint64_t data)
{
    SubchDev *sch;
    VirtIODevice *vdev;
    int cssid, ssid, schid, m;
    uint16_t vq_idx = data;

    if (ioinst_disassemble_sch_ident(subch_id, &m, &cssid, &ssid, &schid)) {
        return -EINVAL;
    }
    sch = css_find_subch(m, cssid, ssid, schid);
    if (!sch || !css_subch_visible(sch)) {
        return -EINVAL;
    }

    vdev = virtio_ccw_get_vdev(sch);
    if (vq_idx >= VIRTIO_QUEUE_MAX || !virtio_queue_get_num(vdev, vq_idx)) {
        return -EINVAL;
    }

    if (virtio_vdev_has_feature(vdev, VIRTIO_F_NOTIFICATION_DATA)) {
        virtio_queue_set_shadow_avail_idx(virtio_get_queue(vdev, vq_idx),
                                          (data >> 16) & 0xFFFF);
    }

    virtio_queue_notify(vdev, vq_idx);
    return 0;
}

static void handle_storage_limits(CPUS390XState *env)
{
    MachineState *machine = MACHINE(qdev_get_machine());

    /* Initial memory. */
    env->regs[2] = machine->ram_size - 1;

    /* Upper limit for hotplug/device memory. */
    if (!machine->device_memory) {
        env->regs[3] = env->regs[2];
    } else {
        env->regs[3] = machine->device_memory->base +
                       memory_region_size(&machine->device_memory->mr) - 1;
    }
}

int handle_diag_500(CPUS390XState *env)
{
    const uint64_t subcode = env->regs[1];

    switch (subcode) {
    case DIAG500_VIRTIO_NOTIFY:
        env->regs[2] = handle_virtio_notify(env->regs[2]);
        return 0;
    case DIAG500_VIRTIO_CCW_NOTIFY:
        env->regs[2] = handle_virtio_ccw_notify(env->regs[2], env->regs[3]);
        return 0;
    case DIAG500_STORAGE_LIMITS:
        handle_storage_limits(env);
        return 0;
    default:
        return -EINVAL;
    }
}
