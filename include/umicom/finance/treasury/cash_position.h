/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_position.h
 *
 * PURPOSE:
 *   Represent an account cash position with settled and projected balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_POSITION_H
#define UMICOM_FINANCE_TREASURY_CASH_POSITION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury cash position data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCashPosition {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    int64_t settled_minor;
    int64_t projected_minor;
} UmiTreasuryCashPosition;
/**
 * Initialise treasury cash position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_position_init(UmiTreasuryCashPosition *value,
    const char *id,
    const char *currency_code,
    int64_t settled_minor,
    int64_t projected_minor);
/**
 * Check that treasury cash position satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_position_valid(const UmiTreasuryCashPosition *value);
/**
 * Provide the treasury cash position delta minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_position_delta_minor(const UmiTreasuryCashPosition *value);
#ifdef __cplusplus
}
#endif
#endif
