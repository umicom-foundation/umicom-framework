/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/interest_accrual.h
 *
 * PURPOSE:
 *   Calculate deterministic simple-interest accrual in minor units for banking balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_INTEREST_ACCRUAL_H
#define UMICOM_FINANCE_BANKING_INTEREST_ACCRUAL_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingInterestAccrual {
    UmiFinancialId id;
    int64_t principal_minor;
    int32_t annual_rate_bps;
    uint32_t days;
    uint32_t day_count_basis;
} UmiBankingInterestAccrual;
UmiStatus umi_banking_interest_accrual_init(UmiBankingInterestAccrual *value,
    const char *id,
    int64_t principal_minor,
    int32_t annual_rate_bps,
    uint32_t days,
    uint32_t day_count_basis);
bool umi_banking_interest_accrual_valid(const UmiBankingInterestAccrual *value);
int64_t umi_banking_interest_accrual_accrued_minor(const UmiBankingInterestAccrual *value);
#ifdef __cplusplus
}
#endif
#endif
