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

/* A display projection retains customer and currency identity. All statuses are
 * included; available funds on a blocked or closed account are not permission
 * to spend. Applications must apply their authorization and payment policy. */
typedef struct UmiBankingDepositSummary {
    UmiFinancialId customer_id;
    UmiCurrency currency;
    size_t account_count;
    size_t pending_count;
    size_t open_count;
    size_t blocked_count;
    size_t closed_count;
    int64_t ledger_minor;
    int64_t available_minor;
    int64_t reserved_minor;
} UmiBankingDepositSummary;

/* Sum accounts for exactly one customer and currency without conversion.
 * The book must contain valid, uniquely identified accounts, including rows
 * outside the selected scope. An empty match succeeds with zero totals.
 * Every intermediate minor-unit sum must fit int64_t; otherwise return
 * CAPACITY_EXCEEDED. Invalid records or duplicate IDs return INVALID_STATE.
 * Bad arguments return INVALID_ARGUMENT. Failures preserve out_summary.
 * Caller serializes book access and supplies a separate output object. */
UmiStatus UmiBankingDepositAccountBookSummarise(
    const UmiBankingDepositAccountBook *book, const char *customer_id,
    const UmiCurrency *currency, UmiBankingDepositSummary *out_summary);

#ifdef __cplusplus
}
#endif
#endif
