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
