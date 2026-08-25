/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/taxonomy_member.h
 *
 * PURPOSE:
 *   Describe an allowed member of a reporting dimension.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_MEMBER_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_MEMBER_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTaxonomyMember {
    char member_id[UMI_REG_ID_CAPACITY];
    char dimension_id[UMI_REG_ID_CAPACITY];
    char label[UMI_REG_NAME_CAPACITY];
    int active;
} UmiTaxonomyMember;

/* Initialise one validated describe an allowed member of a reporting dimension. record. */
UmiStatus umi_reg_taxonomy_member_init(UmiTaxonomyMember *record, const char *member_id, const char *dimension_id, const char *label, int active);

#ifdef __cplusplus
}
#endif

#endif
