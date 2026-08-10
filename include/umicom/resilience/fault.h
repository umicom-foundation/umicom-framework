/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/fault.h
 *
 * PURPOSE:
 *   Classify Framework status values into retryable, permanent, security and
 *   cancellation fault categories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_FAULT_H
#define UMICOM_RESILIENCE_FAULT_H

#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiFaultKind { UMI_FAULT_NONE = 0, UMI_FAULT_TRANSIENT = 1, UMI_FAULT_PERMANENT = 2, UMI_FAULT_SECURITY = 3, UMI_FAULT_CANCELLED = 4 } UmiFaultKind;
UmiFaultKind umi_fault_classify(UmiStatus status);
int umi_fault_retryable(UmiStatus status);
const char *umi_fault_kind_text(UmiFaultKind kind);
#ifdef __cplusplus
}
#endif

#endif
