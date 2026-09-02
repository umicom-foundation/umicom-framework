/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/regulatory_authority.h
 *
 * PURPOSE:
 *   Represent a regulatory authority and jurisdiction boundary.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_AUTHORITY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_AUTHORITY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the regulatory authority data shared with callers of this public contract.
 */
typedef struct UmiRegulatoryAuthority {
    char authority_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    char jurisdiction[UMI_REG_NAME_CAPACITY];
    int active;
} UmiRegulatoryAuthority;

/* Initialise one validated represent a regulatory authority and jurisdiction boundary. record. */
UmiStatus umi_reg_regulatory_authority_init(UmiRegulatoryAuthority *record, const char *authority_id, const char *name, const char *jurisdiction, int active);

#ifdef __cplusplus
}
#endif

#endif
