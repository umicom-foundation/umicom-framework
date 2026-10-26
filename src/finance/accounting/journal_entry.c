/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/journal_entry.c
 *
 * PURPOSE:
 *   Implement bounded journal line insertion, totals and double-entry balance checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_entry.h"
#include <string.h>

/* The former journal implementation is retained for review. Checked totals and complete line validation replace unbounded traversal and signed addition; valid journal behavior remains available through the same public functions. */
#if 0
/*
 * Initialise accounting journal entry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,const char *id,UmiFinancialDate accounting_date){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(value,0,sizeof *value); UmiStatus s=umi_accounting_id_assign(&value->id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; value->accounting_date=accounting_date; value->status=UMI_ACCOUNTING_JOURNAL_DRAFT; return umi_accounting_journal_entry_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT; }
/*
 * Check that accounting journal entry satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value){ return value!=NULL&&umi_financial_id_is_valid(&value->id)&&umi_financial_date_is_valid(value->accounting_date)&&value->line_count<=UMI_ACCOUNTING_MAX_LINES; }
/*
 * Provide the accounting journal entry add line operation used by this module and its
 * client applications.
 */
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,const UmiAccountingJournalLine *line){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_accounting_journal_entry_valid(value)||line==NULL||!umi_accounting_journal_line_valid(line))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->status!=UMI_ACCOUNTING_JOURNAL_DRAFT)return UMI_STATUS_INVALID_STATE; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->line_count>=UMI_ACCOUNTING_MAX_LINES)return UMI_STATUS_CAPACITY_EXCEEDED; value->lines[value->line_count++]=*line; return UMI_STATUS_OK; }
/*
 * Provide the accounting journal entry total debit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value){ int64_t t=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return 0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].debit_minor; return t; }
/*
 * Provide the accounting journal entry total credit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value){ int64_t t=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return 0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].credit_minor; return t; }
/*
 * Provide the accounting journal entry balanced operation used by this module and its
 * client applications.
 */
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value){ return value!=NULL&&value->line_count>=2U&&umi_accounting_journal_entry_total_debit(value)==umi_accounting_journal_entry_total_credit(value); }

#endif

/* Checked traversal belongs to the journal owner: every caller, including
 * restored/plugin state, must use the same bounds and representability rules. */
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,
    const char *id, UmiFinancialDate accounting_date) {
    if (value == NULL || !umi_financial_date_is_valid(accounting_date)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAccountingJournalEntry next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status != UMI_STATUS_OK) return status;
    next.accounting_date = accounting_date; next.status = UMI_ACCOUNTING_JOURNAL_DRAFT;
    if (!umi_accounting_journal_entry_valid(&next)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = next;
    return UMI_STATUS_OK;
}
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value) {
    /* Membership is independent of the compiler's enum signedness. Retain the
     * range draft for review; named values also remain safe if the enum grows. */
#if 0
    if (value == NULL || value->line_count > UMI_ACCOUNTING_MAX_LINES ||
        !umi_financial_id_is_valid(&value->id) || !umi_financial_date_is_valid(value->accounting_date) ||
        value->status < UMI_ACCOUNTING_JOURNAL_DRAFT || value->status > UMI_ACCOUNTING_JOURNAL_REVERSED)
        return false;
#endif
    if (value == NULL || value->line_count > UMI_ACCOUNTING_MAX_LINES ||
        !umi_financial_id_is_valid(&value->id) ||
        !umi_financial_date_is_valid(value->accounting_date)) {
        return false;
    }
    switch (value->status) {
    case UMI_ACCOUNTING_JOURNAL_DRAFT:
    case UMI_ACCOUNTING_JOURNAL_VALIDATED:
    case UMI_ACCOUNTING_JOURNAL_POSTED:
    case UMI_ACCOUNTING_JOURNAL_REVERSED:
        break;
    default:
        return false;
    }
    for (size_t i = 0U; i < value->line_count; ++i)
        if (!umi_accounting_journal_line_valid(&value->lines[i])) return false;
    return true;
}
UmiStatus umi_accounting_journal_entry_totals(const UmiAccountingJournalEntry *value,
    int64_t *out_debit, int64_t *out_credit) {
    if (out_debit == NULL || out_credit == NULL || out_debit == out_credit ||
        !umi_accounting_journal_entry_valid(value)) return UMI_STATUS_INVALID_ARGUMENT;
    int64_t debit = 0, credit = 0;
    for (size_t i = 0U; i < value->line_count; ++i) {
        const UmiAccountingJournalLine *line = &value->lines[i];
        if (line->debit_minor > INT64_MAX - debit || line->credit_minor > INT64_MAX - credit)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        debit += line->debit_minor; credit += line->credit_minor;
    }
    *out_debit = debit; *out_credit = credit;
    return UMI_STATUS_OK;
}
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,
    const UmiAccountingJournalLine *line) {
    if (!umi_accounting_journal_entry_valid(value) || !umi_accounting_journal_line_valid(line))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (value->status != UMI_ACCOUNTING_JOURNAL_DRAFT) return UMI_STATUS_INVALID_STATE;
    if (value->line_count >= UMI_ACCOUNTING_MAX_LINES) return UMI_STATUS_CAPACITY_EXCEEDED;
    int64_t debit = 0, credit = 0;
    UmiStatus status = umi_accounting_journal_entry_totals(value, &debit, &credit);
    if (status != UMI_STATUS_OK) return status;
    if (line->debit_minor > INT64_MAX - debit || line->credit_minor > INT64_MAX - credit)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAccountingJournalLine accepted = *line;
    value->lines[value->line_count] = accepted;
    ++value->line_count;
    return UMI_STATUS_OK;
}
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value) {
    int64_t debit = 0, credit = 0;
    return umi_accounting_journal_entry_totals(value, &debit, &credit) == UMI_STATUS_OK ? debit : 0;
}
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value) {
    int64_t debit = 0, credit = 0;
    return umi_accounting_journal_entry_totals(value, &debit, &credit) == UMI_STATUS_OK ? credit : 0;
}
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value) {
    int64_t debit = 0, credit = 0;
    return umi_accounting_journal_entry_totals(value, &debit, &credit) == UMI_STATUS_OK &&
        value->line_count >= 2U && debit > 0 && debit == credit;
}
