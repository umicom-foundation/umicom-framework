/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/balance.h
 *
 * PURPOSE:
 *   Represent dated financial balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BALANCE_H
#define UMICOM_FINANCE_CORE_BALANCE_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialBalance { UmiFinancialId id; UmiMoney amount; UmiFinancialDate date; uint32_t state; } UmiFinancialBalance;
/* Initialize monetary record. */ UmiStatus umi_balance_init(UmiFinancialBalance *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state);
/* Validate monetary record. */ bool umi_balance_is_valid(const UmiFinancialBalance *x);
#ifdef __cplusplus
}
#endif

#endif
