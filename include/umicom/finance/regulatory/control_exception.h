/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_exception.h
 *
 * PURPOSE:
 *   Represent an approved or open exception to a financial control.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EXCEPTION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EXCEPTION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiControlException {
    char exception_id[UMI_REG_ID_CAPACITY];
    char control_id[UMI_REG_ID_CAPACITY];
    char reason[UMI_REG_TEXT_CAPACITY];
    int approved;
} UmiControlException;

/* Initialise one validated represent an approved or open exception to a financial control. record. */
UmiStatus umi_reg_control_exception_init(UmiControlException *record, const char *exception_id, const char *control_id, const char *reason, int approved);

#ifdef __cplusplus
}
#endif

#endif
