/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/account_hold.h
 *
 * PURPOSE:
 *   Represent ring-fenced account funds and explicit release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_ACCOUNT_HOLD_H
#define UMICOM_FINANCE_BANKING_ACCOUNT_HOLD_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking account hold struct shared with callers of this public
 * contract.
 */
typedef struct UmiBankingAccountHold {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t amount_minor;
    bool active;
} UmiBankingAccountHold;
/**
 * Initialise banking account hold from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active);
/**
 * Check that banking account hold satisfies its contract before another
 * service relies on it.
 */
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value);
/**
 * Provide the banking account hold releasable operation used by this module
 * and its client applications.
 */
bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value);
#ifdef __cplusplus
}
#endif
#endif
