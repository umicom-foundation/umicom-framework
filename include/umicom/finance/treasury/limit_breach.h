/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/limit_breach.h
 *
 * PURPOSE:
 *   Record risk-limit breaches and acknowledgement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIMIT_BREACH_H
#define UMICOM_FINANCE_TREASURY_LIMIT_BREACH_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury limit breach data shared with callers of this public contract.
 */
typedef struct UmiTreasuryLimitBreach {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t excess_minor;
    bool acknowledged;
} UmiTreasuryLimitBreach;
/**
 * Initialise treasury limit breach from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_limit_breach_init(UmiTreasuryLimitBreach *value,
    const char *id,
    int64_t excess_minor,
    bool acknowledged);
/**
 * Check that treasury limit breach satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_limit_breach_valid(const UmiTreasuryLimitBreach *value);
/**
 * Provide the treasury limit breach open operation used by this module and its client
 * applications.
 */
bool umi_treasury_limit_breach_open(const UmiTreasuryLimitBreach *value);
#ifdef __cplusplus
}
#endif
#endif
