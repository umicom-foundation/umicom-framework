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
typedef struct UmiBankingAccountHold {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t amount_minor;
    bool active;
} UmiBankingAccountHold;
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active);
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value);
bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value);
#ifdef __cplusplus
}
#endif
#endif
