/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_owner.h
 *
 * PURPOSE:
 *   Record accountable business ownership for a governed data domain.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_OWNER_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_OWNER_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataOwner {
    char owner_id[UMI_REG_ID_CAPACITY];
    char domain_id[UMI_REG_ID_CAPACITY];
    char display_name[UMI_REG_NAME_CAPACITY];
    int active;
} UmiDataOwner;

/* Initialise one validated record accountable business ownership for a governed data domain. record. */
UmiStatus umi_reg_data_owner_init(UmiDataOwner *record, const char *owner_id, const char *domain_id, const char *display_name, int active);

#ifdef __cplusplus
}
#endif

#endif
