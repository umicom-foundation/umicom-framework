/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/qmp_channel.h
 * PURPOSE: Connect QEMU monitor messages to the owned process channel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Provider adapter boundary for an already owned process channel. Production
 * UI uses UmiVmStart, which also checks the reviewed runtime/image. This entry
 * permits other native hosts and tests to exercise QMP without an OS image. */
#ifndef UMICOM_VM_MANAGER_QMP_CHANNEL_H
#define UMICOM_VM_MANAGER_QMP_CHANNEL_H
#include "umicom/vm_manager/manager.h"
#ifdef __cplusplus
extern "C" {
#endif
    /** Consumes channel ownership only on success. Negotiates QMP and queries
                         * state, but never resumes a guest. Caller destroys channel after failure. */
    UmiStatus UmiVmQmpAdoptChannel(UmiProcessChannel *channel,     UmiVmSession **outSession,UmiVmReport *report);
#ifdef __cplusplus
}
#endif
#endif
