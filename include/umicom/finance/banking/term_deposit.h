/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/term_deposit.h
 *
 * PURPOSE:
 *   Represent principal, maturity and rollover intent for term deposits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#define UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingTermDeposit {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    int64_t principal_minor;
    UmiFinancialDate start_date;
    UmiFinancialDate maturity_date;
    bool rollover;
} UmiBankingTermDeposit;
UmiStatus umi_banking_term_deposit_init(UmiBankingTermDeposit *value,
    const char *id,
    const char *customer_id,
    int64_t principal_minor,
    UmiFinancialDate start_date,
    UmiFinancialDate maturity_date,
    bool rollover);
bool umi_banking_term_deposit_valid(const UmiBankingTermDeposit *value);
bool umi_banking_term_deposit_auto_rollover(const UmiBankingTermDeposit *value);
#ifdef __cplusplus
}
#endif
#endif
