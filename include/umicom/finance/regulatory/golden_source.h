/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/golden_source.h
 *
 * PURPOSE:
 *   Designate an authoritative golden source for a governed data domain.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_GOLDEN_SOURCE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_GOLDEN_SOURCE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGoldenSource {
    char domain_id[UMI_REG_ID_CAPACITY];
    char source_id[UMI_REG_ID_CAPACITY];
    uint32_t priority;
    int active;
} UmiGoldenSource;

/* Initialise one validated designate an authoritative golden source for a governed data domain. record. */
UmiStatus umi_reg_golden_source_init(UmiGoldenSource *record, const char *domain_id, const char *source_id, uint32_t priority, int active);

#ifdef __cplusplus
}
#endif

#endif
