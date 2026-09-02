/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/fault.c
 *
 * PURPOSE:
 *   Implement stable status-to-fault classification shared by retry, supervision
 *   and diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/fault.h"
/* Provide the fault classify operation used by this module and its client applications. */
UmiFaultKind umi_fault_classify(UmiStatus status)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) return UMI_FAULT_NONE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) return UMI_FAULT_CANCELLED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_PERMISSION_DENIED) return UMI_FAULT_SECURITY;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_UNAVAILABLE || status == UMI_STATUS_BUSY || status == UMI_STATUS_TIMEOUT || status == UMI_STATUS_IO_ERROR) return UMI_FAULT_TRANSIENT;
    return UMI_FAULT_PERMANENT;
}
/* Provide the fault retryable operation used by this module and its client applications. */
int umi_fault_retryable(UmiStatus status) { return umi_fault_classify(status) == UMI_FAULT_TRANSIENT; }
/* Provide the fault kind text operation used by this module and its client applications. */
const char *umi_fault_kind_text(UmiFaultKind kind)
{ /* Select the behaviour associated with the requested command or state value. */ switch (kind) { case UMI_FAULT_NONE: return "none"; case UMI_FAULT_TRANSIENT: return "transient"; case UMI_FAULT_PERMANENT: return "permanent"; case UMI_FAULT_SECURITY: return "security"; case UMI_FAULT_CANCELLED: return "cancelled"; default: return "unknown"; } }
