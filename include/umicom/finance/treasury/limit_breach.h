/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/limit_breach.h
 *
 * PURPOSE:
 *   Record risk-limit breaches and acknowledgement state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIMIT_BREACH_H
#define UMICOM_FINANCE_TREASURY_LIMIT_BREACH_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLimitBreach {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t excess_minor;
    bool acknowledged;
} UmiTreasuryLimitBreach;
UmiStatus umi_treasury_limit_breach_init(UmiTreasuryLimitBreach *value,
    const char *id,
    int64_t excess_minor,
    bool acknowledged);
bool umi_treasury_limit_breach_valid(const UmiTreasuryLimitBreach *value);
bool umi_treasury_limit_breach_open(const UmiTreasuryLimitBreach *value);
#ifdef __cplusplus
}
#endif
#endif
