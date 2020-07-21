/*
 * Support for QEMU/KVM-specific hypercalls on s390
 *
 * Copyright IBM Corp. 2012, 2017
 * Author(s): Cornelia Huck <cornelia.huck@de.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */

#ifndef HW_S390_HYPERCALL_H
#define HW_S390_HYPERCALL_H

#define DIAG500_VIRTIO_NOTIFY          0 /* legacy, implemented as a NOP */
#define DIAG500_VIRTIO_RESET           1 /* legacy */
#define DIAG500_VIRTIO_SET_STATUS      2 /* legacy */
#define DIAG500_VIRTIO_CCW_NOTIFY      3 /* KVM_S390_VIRTIO_CCW_NOTIFY */

void handle_diag_500(CPUS390XState *env, uintptr_t ra);
#endif /* HW_S390_HYPERCALL_H */
