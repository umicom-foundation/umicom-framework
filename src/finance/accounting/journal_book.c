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
void umi_accounting_journal_book_init(UmiAccountingJournalBook *value){ if(value!=NULL)memset(value,0,sizeof *value); }
const UmiAccountingJournalEntry *umi_accounting_journal_book_find(const UmiAccountingJournalBook *value,const char *id){ if(value==NULL||id==NULL)return NULL; UmiFinancialId k; if(umi_financial_id_assign(&k,id)!=UMI_STATUS_OK)return NULL; for(size_t i=0U;i<value->count;++i)if(umi_financial_id_compare(&value->items[i].id,&k)==0)return &value->items[i]; return NULL; }
UmiStatus umi_accounting_journal_book_post(UmiAccountingJournalBook *value,const UmiAccountingJournalEntry *entry){ if(value==NULL||entry==NULL||!umi_accounting_journal_entry_valid(entry)||!umi_accounting_journal_entry_balanced(entry))return UMI_STATUS_INVALID_ARGUMENT; if(value->count>=UMI_ACCOUNTING_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; if(umi_accounting_journal_book_find(value,entry->id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS; value->items[value->count]=*entry; value->items[value->count].status=UMI_ACCOUNTING_JOURNAL_POSTED; ++value->count; return UMI_STATUS_OK; }
