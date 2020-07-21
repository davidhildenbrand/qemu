/*
 * Support for QEMU/KVM-specific hypercalls on s390
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
#include "hw/s390x/s390-hypercall.h"
#include "hw/s390x/ioinst.h"
#include "hw/s390x/css.h"
#include "virtio-ccw.h"

static int handle_virtio_notify(uint64_t mem)
{
    if (mem < ram_size) {
        /* Tolerate early printk. */
        return 0;
    }
    return -EINVAL;
}

static int handle_virtio_ccw_notify(uint64_t subch_id, uint64_t queue)
{
    SubchDev *sch;
    int cssid, ssid, schid, m;

    if (ioinst_disassemble_sch_ident(subch_id, &m, &cssid, &ssid, &schid)) {
        return -EINVAL;
    }
    sch = css_find_subch(m, cssid, ssid, schid);
    if (!sch || !css_subch_visible(sch)) {
        return -EINVAL;
    }
    if (queue >= VIRTIO_QUEUE_MAX) {
        return -EINVAL;
    }
    virtio_queue_notify(virtio_ccw_get_vdev(sch), queue);
    return 0;
}

void handle_diag_500(CPUS390XState *env, uintptr_t ra)
{
     const uint64_t subcode = env->regs[1];

     switch (subcode) {
     case DIAG500_VIRTIO_NOTIFY:
         env->regs[2] = handle_virtio_notify(env->regs[2]);
         break;
     case DIAG500_VIRTIO_CCW_NOTIFY:
         env->regs[2] = handle_virtio_ccw_notify(env->regs[2], env->regs[3]);
         break;
     default:
         s390_program_interrupt(env, PGM_SPECIFICATION, ra);
     }
}
