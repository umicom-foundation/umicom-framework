/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/source_system.h
 *
 * PURPOSE:
 *   Register a source system and extraction criticality for governed data.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SOURCE_SYSTEM_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SOURCE_SYSTEM_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSourceSystem {
    char source_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    char owner_id[UMI_REG_ID_CAPACITY];
    int critical;
} UmiSourceSystem;

/* Initialise one validated register a source system and extraction criticality for governed data. record. */
UmiStatus umi_reg_source_system_init(UmiSourceSystem *record, const char *source_id, const char *name, const char *owner_id, int critical);

#ifdef __cplusplus
}
#endif

#endif
