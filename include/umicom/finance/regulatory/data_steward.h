/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_steward.h
 *
 * PURPOSE:
 *   Record operational stewardship responsibility for governed data.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_STEWARD_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_STEWARD_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSteward {
    char steward_id[UMI_REG_ID_CAPACITY];
    char domain_id[UMI_REG_ID_CAPACITY];
    char display_name[UMI_REG_NAME_CAPACITY];
    int active;
} UmiDataSteward;

/* Initialise one validated record operational stewardship responsibility for governed data. record. */
UmiStatus umi_reg_data_steward_init(UmiDataSteward *record, const char *steward_id, const char *domain_id, const char *display_name, int active);

#ifdef __cplusplus
}
#endif

#endif
