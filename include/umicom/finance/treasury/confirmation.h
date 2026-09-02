/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/confirmation.h
 *
 * PURPOSE:
 *   Represent trade confirmation terms and confirmation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#define UMICOM_FINANCE_TREASURY_CONFIRMATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury confirmation data shared with callers of this public contract.
 */
typedef struct UmiTreasuryConfirmation {
    char id[UMI_TREASURY_ID_CAPACITY];
    char trade_id[UMI_TREASURY_ID_CAPACITY];
    bool sent;
    bool acknowledged;
} UmiTreasuryConfirmation;
/**
 * Initialise treasury confirmation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged);
/**
 * Check that treasury confirmation satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value);
/**
 * Provide the treasury confirmation complete operation used by this module and its client
 * applications.
 */
bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value);
#ifdef __cplusplus
}
#endif
#endif
