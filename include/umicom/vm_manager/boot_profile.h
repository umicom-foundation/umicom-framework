/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/boot_profile.h
 * PURPOSE: Store named standalone boot requests in the canonical local Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VM_MANAGER_BOOT_PROFILE_H
#define UMICOM_VM_MANAGER_BOOT_PROFILE_H
#include "umicom/vm_manager/boot.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVmBootProfile {
    char id[64];
    char name[128];
    UmiVmBootRequest request;
    uint64_t revision;
} UmiVmBootProfile;

typedef UmiStatus (*UmiVmBootProfileVisitor)(const UmiVmBootProfile *profile, void *context);

/* Profiles are configuration, not running sessions or saved review approvals.
 * Saving validates the request but never reads its referenced files. A later
 * launch must build a plan and review the actual inputs again.
 * The local Data Server is borrowed and may not be used concurrently or carry
 * a caller transaction. expectedRevision=0 creates; other values replace only
 * that exact revision. Conflicting updates leave the stored record untouched. */
UmiStatus UmiVmBootProfileSave(UmiDataServer *server, const UmiVmBootProfile *profile,
                             uint64_t expectedRevision, uint64_t *outRevision);
/* Failed reads leave the output object unchanged. Visitors receive a borrowed
 * object only for that callback; do not re-enter or mutate this connection. */
UmiStatus UmiVmBootProfileLoad(const UmiDataServer *server, const char *id,
                             UmiVmBootProfile *outProfile);
UmiStatus UmiVmBootProfileVisit(const UmiDataServer *server,
                              UmiVmBootProfileVisitor visitor, void *context);
UmiStatus UmiVmBootProfileRemove(UmiDataServer *server, const char *id,
                               uint64_t expectedRevision);
#ifdef __cplusplus
}
#endif
#endif
