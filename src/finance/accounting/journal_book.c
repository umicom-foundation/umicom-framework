/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/journal_book.c
 *
 * PURPOSE:
 *   Implement balanced journal posting, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_book.h"
#include <string.h>

/* Retain the original journal storage for review. Bounded complete-book validation now guards lookup and posting before any traversal or publication; accepted entries remain copied into the same in-memory book. */
#if 0
/*
 * Initialise accounting journal book from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_journal_book_init(UmiAccountingJournalBook *value){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL)memset(value,0,sizeof *value); }
/*
 * Find accounting journal book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAccountingJournalEntry *umi_accounting_journal_book_find(const UmiAccountingJournalBook *value,const char *id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||id==NULL)return NULL; UmiFinancialId k; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_id_assign(&k,id)!=UMI_STATUS_OK)return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_id_compare(&value->items[i].id,&k)==0)return &value->items[i]; return NULL; }
/*
 * Provide the accounting journal book post operation used by this module and its client
 * applications.
 */
UmiStatus umi_accounting_journal_book_post(UmiAccountingJournalBook *value,const UmiAccountingJournalEntry *entry){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||entry==NULL||!umi_accounting_journal_entry_valid(entry)||!umi_accounting_journal_entry_balanced(entry))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->count>=UMI_ACCOUNTING_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_accounting_journal_book_find(value,entry->id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS; value->items[value->count]=*entry; value->items[value->count].status=UMI_ACCOUNTING_JOURNAL_POSTED; ++value->count; return UMI_STATUS_OK; }

#endif

/* Validate public counts and every stored entry before lookup or publication.
 * The bounded in-memory book is not a durable ledger or an approval service. */
UmiStatus umi_accounting_journal_book_validate(const UmiAccountingJournalBook *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->count > UMI_ACCOUNTING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < value->count; ++i) {
        if (!umi_accounting_journal_entry_balanced(&value->items[i]) ||
            value->items[i].status != UMI_ACCOUNTING_JOURNAL_POSTED) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (umi_financial_id_compare(&value->items[i].id, &value->items[j].id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_accounting_journal_book_init(UmiAccountingJournalBook *value) {
    if (value != NULL) memset(value, 0, sizeof *value);
}
const UmiAccountingJournalEntry *umi_accounting_journal_book_find(
    const UmiAccountingJournalBook *value, const char *id) {
    if (umi_accounting_journal_book_validate(value) != UMI_STATUS_OK) return NULL;
    UmiFinancialId key = {0};
    if (umi_financial_id_assign(&key, id) != UMI_STATUS_OK || !umi_financial_id_is_valid(&key)) return NULL;
    for (size_t i = 0U; i < value->count; ++i)
        if (umi_financial_id_compare(&value->items[i].id, &key) == 0) return &value->items[i];
    return NULL;
}
UmiStatus umi_accounting_journal_book_post(UmiAccountingJournalBook *value,
    const UmiAccountingJournalEntry *entry) {
    if (value == NULL || !umi_accounting_journal_entry_balanced(entry)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_accounting_journal_book_validate(value);
    if (status != UMI_STATUS_OK) return status;
    if (value->count >= UMI_ACCOUNTING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_accounting_journal_book_find(value, entry->id.value) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    UmiAccountingJournalEntry accepted = *entry;
    accepted.status = UMI_ACCOUNTING_JOURNAL_POSTED;
    value->items[value->count] = accepted;
    ++value->count; return UMI_STATUS_OK;
}
