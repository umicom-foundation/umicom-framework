/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/financial_account.h
 *
 * PURPOSE:
 *   Define richer account metadata without replacing existing UmiFinancialAccount.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FINANCIAL_ACCOUNT_H
#define UMICOM_FINANCE_CORE_FINANCIAL_ACCOUNT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialCoreAccount { UmiFinancialId account_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiFinancialCoreAccount;
/* Initialize the typed financial record. */ UmiStatus umi_financial_account_init(UmiFinancialCoreAccount *item,const char *id,const char *name,const char *parent_id,const char *code);
/* Validate the typed financial record. */ bool umi_financial_account_is_valid(const UmiFinancialCoreAccount *item);
#ifdef __cplusplus
}
#endif

#endif
