/*
 * Virtio MEM device
 *
 * Copyright (C) 2020 Red Hat, Inc.
 *
 * Authors:
 *  David Hildenbrand <david@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.
 * See the COPYING file in the top-level directory.
 */

#ifndef HW_VIRTIO_MEM_H
#define HW_VIRTIO_MEM_H

#include "standard-headers/linux/virtio_mem.h"
#include "hw/virtio/virtio.h"
#include "qapi/qapi-types-misc.h"
#include "sysemu/hostmem.h"
#include "qom/object.h"

#define TYPE_VIRTIO_MEM "virtio-mem"

OBJECT_DECLARE_TYPE(VirtIOMEM, VirtIOMEMClass,
                    VIRTIO_MEM)

#define VIRTIO_MEM_MEMDEV_PROP "memdev"
#define VIRTIO_MEM_NODE_PROP "node"
#define VIRTIO_MEM_SIZE_PROP "size"
#define VIRTIO_MEM_REQUESTED_SIZE_PROP "requested-size"
#define VIRTIO_MEM_BLOCK_SIZE_PROP "block-size"
#define VIRTIO_MEM_ADDR_PROP "memaddr"

struct VirtIOMEM {
    VirtIODevice parent_obj;

    /* guest -> host request queue */
    VirtQueue *vq;

    /* bitmap used to track unplugged memory */
    int32_t bitmap_size;
    unsigned long *bitmap;

    /* assigned memory backend and memory region */
    HostMemoryBackend *memdev;

    /* NUMA node */
    uint32_t node;

    /* assigned address of the region in guest physical memory */
    uint64_t addr;

    /* usable region size (<= region_size) */
    uint64_t usable_region_size;

    /* actual size (how much the guest plugged) */
    uint64_t size;

    /* requested size */
    uint64_t requested_size;

    /* block size and alignment */
    uint64_t block_size;

    /*
     * While this mutex is locked, the state of blocks can't change; the size and
     * the bitmap can't change, and no plugged memory will get discarded.
     *
     * Besides system resets, only the request thread changes the size, the
     * bitmap and the usable_region_size. During system resets, we temporarily
     * pause the request thread to avoid locking.
     *
     * Only the main thread changes the requested_size. It pauses the request
     * thread before doing so, to avoid locking.
     */
    QemuMutex mutex;

    struct {
        /* Request thread that processes guests requests in the virtqueue. */
        QemuThread thread;

        /*
         * Protects cond, destroying and paused. The request thread grabs this
         * mutex while active. Also protects the rdl_list from concurrent
         * notifications while register/unregistering listeners.
         */
        QemuMutex mutex;

        /* Condition the request thread waits for. */
        QemuCond cond;

        /* Make the request thread quit so we can destroy it. */
        bool destroying;

        /* Make the request thread pause processing requests until unpaused. */
        bool paused;
    } req;

    /* notifiers to notify when "size" changes */
    NotifierList size_change_notifiers;

    /* don't migrate unplugged memory */
    NotifierWithReturn precopy_notifier;

    /* listeners to notify on plug/unplug activity. */
    QLIST_HEAD(, RamDiscardListener) rdl_list;
};

struct VirtIOMEMClass {
    /* private */
    VirtIODevice parent;

    /* public */
    void (*fill_device_info)(const VirtIOMEM *vmen, VirtioMEMDeviceInfo *vi);
    MemoryRegion *(*get_memory_region)(VirtIOMEM *vmem, Error **errp);
    void (*add_size_change_notifier)(VirtIOMEM *vmem, Notifier *notifier);
    void (*remove_size_change_notifier)(VirtIOMEM *vmem, Notifier *notifier);
};

#endif
