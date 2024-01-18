#ifndef QEMU_SYSEMU_RESET_H
#define QEMU_SYSEMU_RESET_H

#include "qapi/qapi-events-run-state.h"

typedef void QEMUResetHandler(void *opaque);
typedef void QEMUResetReasonHandler(void *opaque, ShutdownCause reason);

void qemu_register_reset(QEMUResetHandler *func, void *opaque);
void qemu_register_reset_nosnapshotload(QEMUResetHandler *func, void *opaque);
void qemu_unregister_reset(QEMUResetHandler *func, void *opaque);
void qemu_register_reset_reason(QEMUResetReasonHandler *func, void *opaque);
void qemu_unregister_reset_reason(QEMUResetReasonHandler *func, void *opaque);
void qemu_devices_reset(ShutdownCause reason);

#endif
