/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/deposit_account_book.h
 *
 * PURPOSE:
 *   Maintain bounded deposit accounts for reusable banking services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_BOOK_H
#define UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_BOOK_H
#include "umicom/finance/banking/deposit_account.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking deposit account book data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingDepositAccountBook { UmiBankingDepositAccount items[UMI_BANKING_MAX_ITEMS]; size_t count; } UmiBankingDepositAccountBook;
/**
 * Initialise banking deposit account book from caller-provided values so later operations
 * receive a known state.
 */
void umi_banking_deposit_account_book_init(UmiBankingDepositAccountBook *registry);
/**
 * Add banking deposit account book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_banking_deposit_account_book_add(UmiBankingDepositAccountBook *registry,const UmiBankingDepositAccount *item);
/**
 * Find banking deposit account book while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiBankingDepositAccount *umi_banking_deposit_account_book_find(const UmiBankingDepositAccountBook *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
