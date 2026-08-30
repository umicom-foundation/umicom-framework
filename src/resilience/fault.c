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
UmiFaultKind umi_fault_classify(UmiStatus status)
{
    if (status == UMI_STATUS_OK) return UMI_FAULT_NONE;
    if (status == UMI_STATUS_CANCELLED) return UMI_FAULT_CANCELLED;
    if (status == UMI_STATUS_PERMISSION_DENIED) return UMI_FAULT_SECURITY;
    if (status == UMI_STATUS_UNAVAILABLE || status == UMI_STATUS_BUSY || status == UMI_STATUS_TIMEOUT || status == UMI_STATUS_IO_ERROR) return UMI_FAULT_TRANSIENT;
    return UMI_FAULT_PERMANENT;
}
int umi_fault_retryable(UmiStatus status) { return umi_fault_classify(status) == UMI_FAULT_TRANSIENT; }
const char *umi_fault_kind_text(UmiFaultKind kind)
{ switch (kind) { case UMI_FAULT_NONE: return "none"; case UMI_FAULT_TRANSIENT: return "transient"; case UMI_FAULT_PERMANENT: return "permanent"; case UMI_FAULT_SECURITY: return "security"; case UMI_FAULT_CANCELLED: return "cancelled"; default: return "unknown"; } }
