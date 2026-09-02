/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_domain.h
 *
 * PURPOSE:
 *   Describe a governed financial data domain.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_DOMAIN_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_DOMAIN_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data domain data shared with callers of this public contract.
 */
typedef struct UmiDataDomain {
    char domain_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    char classification[UMI_REG_NAME_CAPACITY];
    int regulated;
} UmiDataDomain;

/* Initialise one validated describe a governed financial data domain. record. */
UmiStatus umi_reg_data_domain_init(UmiDataDomain *record, const char *domain_id, const char *name, const char *classification, int regulated);

#ifdef __cplusplus
}
#endif

#endif
