/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/term_deposit.h
 *
 * PURPOSE:
 *   Represent principal, maturity and rollover intent for term deposits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#define UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking term deposit data shared with callers of this public contract.
 */
typedef struct UmiBankingTermDeposit {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    int64_t principal_minor;
    UmiFinancialDate start_date;
    UmiFinancialDate maturity_date;
    bool rollover;
} UmiBankingTermDeposit;
/**
 * Initialise banking term deposit from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_term_deposit_init(UmiBankingTermDeposit *value,
    const char *id,
    const char *customer_id,
    int64_t principal_minor,
    UmiFinancialDate start_date,
    UmiFinancialDate maturity_date,
    bool rollover);
/**
 * Check that banking term deposit satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_term_deposit_valid(const UmiBankingTermDeposit *value);
/**
 * Provide the banking term deposit auto rollover operation used by this module and its
 * client applications.
 */
bool umi_banking_term_deposit_auto_rollover(const UmiBankingTermDeposit *value);
#ifdef __cplusplus
}
#endif
#endif
