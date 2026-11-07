/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_account_book.c
 *
 * PURPOSE:
 *   Implement maintain bounded deposit accounts for reusable banking services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account_book.h"
#include <string.h>
#include "umicom/finance/currency.h"
#include "umicom/finance/money.h"
/*
 * Initialise banking deposit account book from caller-provided values so later operations
 * receive a known state.
 */
void umi_banking_deposit_account_book_init(UmiBankingDepositAccountBook *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry!=NULL) memset(registry,0,sizeof *registry); }
/*
 * Find banking deposit account book while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiBankingDepositAccount *umi_banking_deposit_account_book_find(const UmiBankingDepositAccountBook *registry,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL||id==NULL||registry->count>UMI_BANKING_MAX_ITEMS) return NULL;
    UmiFinancialId key;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK || !umi_financial_id_is_valid(&key)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(size_t i=0U;i<registry->count;++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(umi_financial_id_is_valid(&registry->items[i].id) && umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
/*
 * Add banking deposit account book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_banking_deposit_account_book_add(UmiBankingDepositAccountBook *registry,const UmiBankingDepositAccount *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL||item==NULL||!umi_banking_deposit_account_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->count>=UMI_BANKING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_banking_deposit_account_book_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}

/* Framework owns scoped balance projection so desktop, web and mobile clients
 * do not each invent aggregation or mix currencies in their account headers.
 * Canonical money arithmetic checks each integer operation before publishing. */
UmiStatus UmiBankingDepositAccountBookSummarise(
    const UmiBankingDepositAccountBook *book, const char *customer_id,
    const UmiCurrency *currency, UmiBankingDepositSummary *out_summary)
{
    UmiBankingDepositSummary candidate = {0};
    UmiMoney ledger = {0}, available = {0}, reserved = {0};
    if (book == NULL || customer_id == NULL || currency == NULL ||
        out_summary == NULL || !umi_currency_valid(currency))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_banking_id_assign(&candidate.customer_id, customer_id) != UMI_STATUS_OK ||
        !umi_financial_id_is_valid(&candidate.customer_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_BANKING_MAX_ITEMS) return UMI_STATUS_INVALID_STATE;
    candidate.currency = *currency;
    /* These values are already minor-unit counts. Scale zero is internal to
     * checked addition; the projection does not infer a currency's decimals. */
    ledger.currency = available.currency = reserved.currency = *currency;
    for (size_t i = 0; i < book->count; ++i) {
        const UmiBankingDepositAccount *account = &book->items[i];
        if (!umi_banking_deposit_account_valid(account)) return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0; earlier < i; ++earlier) {
            if (umi_financial_id_compare(&book->items[earlier].id, &account->id) == 0)
                return UMI_STATUS_INVALID_STATE;
        }
        if (umi_financial_id_compare(&account->customer_id, &candidate.customer_id) != 0 ||
            !umi_banking_currency_equal(account->currency, *currency)) continue;
        UmiMoney row = {0};
        row.currency = *currency;
        row.minor_units = account->ledger_minor;
        UmiStatus status = umi_money_add(&ledger, &row, &ledger);
        if (status != UMI_STATUS_OK) return status;
        row.minor_units = account->available_minor;
        status = umi_money_add(&available, &row, &available);
        if (status != UMI_STATUS_OK) return status;
        status = UmiBankingDepositAccountReserved(account, &row.minor_units);
        if (status != UMI_STATUS_OK) return status;
        status = umi_money_add(&reserved, &row, &reserved);
        if (status != UMI_STATUS_OK) return status;
        ++candidate.account_count;
        switch (account->status) {
        case UMI_BANKING_ACCOUNT_PENDING: ++candidate.pending_count; break;
        case UMI_BANKING_ACCOUNT_OPEN: ++candidate.open_count; break;
        case UMI_BANKING_ACCOUNT_BLOCKED: ++candidate.blocked_count; break;
        case UMI_BANKING_ACCOUNT_CLOSED: ++candidate.closed_count; break;
        }
    }
    candidate.ledger_minor = ledger.minor_units;
    candidate.available_minor = available.minor_units;
    candidate.reserved_minor = reserved.minor_units;
    *out_summary = candidate;
    return UMI_STATUS_OK;
}
