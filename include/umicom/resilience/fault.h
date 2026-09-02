/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/fault.h
 *
 * PURPOSE:
 *   Classify Framework status values into retryable, permanent, security and
 *   cancellation fault categories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_FAULT_H
#define UMICOM_RESILIENCE_FAULT_H

#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named fault kind values accepted by this public contract.
 */
typedef enum UmiFaultKind { UMI_FAULT_NONE = 0, UMI_FAULT_TRANSIENT = 1, UMI_FAULT_PERMANENT = 2, UMI_FAULT_SECURITY = 3, UMI_FAULT_CANCELLED = 4 } UmiFaultKind;
/**
 * Provide the fault classify operation used by this module and its client applications.
 */
UmiFaultKind umi_fault_classify(UmiStatus status);
/**
 * Provide the fault retryable operation used by this module and its client applications.
 */
int umi_fault_retryable(UmiStatus status);
/**
 * Provide the fault kind text operation used by this module and its client applications.
 */
const char *umi_fault_kind_text(UmiFaultKind kind);
#ifdef __cplusplus
}
#endif

#endif
