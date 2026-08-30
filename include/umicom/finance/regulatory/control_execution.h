/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_execution.h
 *
 * PURPOSE:
 *   Record one control execution and deterministic outcome.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EXECUTION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EXECUTION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiControlExecution {
    char execution_id[UMI_REG_ID_CAPACITY];
    char control_id[UMI_REG_ID_CAPACITY];
    int64_t executed_ms;
    int passed;
} UmiControlExecution;

/* Initialise one validated record one control execution and deterministic outcome. record. */
UmiStatus umi_reg_control_execution_init(UmiControlExecution *record, const char *execution_id, const char *control_id, int64_t executed_ms, int passed);

#ifdef __cplusplus
}
#endif

#endif
