/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_remediation.h
 *
 * PURPOSE:
 *   Represent owned remediation for a failed control or exception.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_REMEDIATION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_REMEDIATION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiControlRemediation {
    char remediation_id[UMI_REG_ID_CAPACITY];
    char exception_id[UMI_REG_ID_CAPACITY];
    char owner_id[UMI_REG_ID_CAPACITY];
    int64_t due_ms;
} UmiControlRemediation;

/* Initialise one validated represent owned remediation for a failed control or exception. record. */
UmiStatus umi_reg_control_remediation_init(UmiControlRemediation *record, const char *remediation_id, const char *exception_id, const char *owner_id, int64_t due_ms);

#ifdef __cplusplus
}
#endif

#endif
