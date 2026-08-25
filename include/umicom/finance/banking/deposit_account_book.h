/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/deposit_account_book.h
 *
 * PURPOSE:
 *   Maintain bounded deposit accounts for reusable banking services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_BOOK_H
#define UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_BOOK_H
#include "umicom/finance/banking/deposit_account.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingDepositAccountBook { UmiBankingDepositAccount items[UMI_BANKING_MAX_ITEMS]; size_t count; } UmiBankingDepositAccountBook;
void umi_banking_deposit_account_book_init(UmiBankingDepositAccountBook *registry);
UmiStatus umi_banking_deposit_account_book_add(UmiBankingDepositAccountBook *registry,const UmiBankingDepositAccount *item);
const UmiBankingDepositAccount *umi_banking_deposit_account_book_find(const UmiBankingDepositAccountBook *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
