/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/account.c
 *
 * PURPOSE:
 *   Validate financial account identity, owner and settlement currency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test account behaviour without starting a complete product.
 */

#include "umicom/finance/account.h"
#include "umicom/finance/identifier.h"
#include "umicom/finance/currency.h"
/* Check that financial account satisfies its contract before another service relies on it. */
int umi_financial_account_valid(const UmiFinancialAccount *account){return account!=NULL && umi_financial_id_valid(&account->account_id) && umi_financial_id_valid(&account->party_id) && umi_currency_valid(&account->currency);}
