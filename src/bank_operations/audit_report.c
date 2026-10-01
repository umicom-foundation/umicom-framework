/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/audit_report.c
 * PURPOSE: Join copied audit events to exact posting revisions without reloading or modifying the ledger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "audit_private.h"
#include "internal.h"
#include "umicom/finance/money_text.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
/* Check the captured journal with bounded text and checked totals before any
 * report formatter reads it. No total is accumulated across currencies. */
static UmiStatus AuditJournalValid(const UmiBankJournal *journal,const UmiBankAuditEvent *event)
{
    if(!BankIdValid(&journal->entry.id,true) || !BankIdValid(&journal->referenceId,true) ||
        strcmp(journal->referenceId.value,event->command.id.value)!=0 || journal->revision!=event->revision ||
        umi_financial_date_compare(journal->entry.accounting_date,event->command.businessDate)!=0 ||
        journal->entry.status!=UMI_ACCOUNTING_JOURNAL_POSTED || journal->scale>9U || !journal->entry.line_count || journal->entry.line_count>UMI_ACCOUNTING_MAX_LINES)
        return UMI_STATUS_INVALID_STATE;
    int64_t debit=0,credit=0;
    for(size_t i=0;i<journal->entry.line_count;++i){
        const UmiAccountingJournalLine *line=&journal->entry.lines[i];
        /* Posted lines include Framework-owned sys.* clearing accounts. The
         * user-command ID policy intentionally excludes that reserved namespace. */
        if(!BankIdValid(&line->id,true) || !umi_financial_id_is_valid(&line->account_id) || line->debit_minor<0 || line->credit_minor<0 ||
            (line->debit_minor==0)==(line->credit_minor==0))return UMI_STATUS_INVALID_STATE;
        if(debit>INT64_MAX-line->debit_minor || credit>INT64_MAX-line->credit_minor)return UMI_STATUS_CAPACITY_EXCEEDED;
        debit+=line->debit_minor;credit+=line->credit_minor;
    }
    UmiMoney amount={0};amount.currency=journal->currency;amount.scale=journal->scale;amount.minor_units=debit;
    char money[UMI_MONEY_TEXT_CAPACITY];
    return debit==credit && debit>0 && UmiMoneyTextFormat(&amount,money,sizeof money,NULL)==UMI_STATUS_OK?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
UmiStatus UmiBankAuditCapture(const UmiBankOperations *operations,const UmiBankAuditQuery *query,UmiBankAuditReport **outReport)
{
    if(outReport==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=NULL;UmiStatus status=UmiBankAuditQueryValidate(query);if(status!=UMI_STATUS_OK)return status;
    UmiBankCounts counts;status=UmiBankOperationsCounts(operations,&counts);if(status!=UMI_STATUS_OK)return status;
    if(counts.events>UMI_BANK_EVENT_CAPACITY || counts.journals>UMI_BANK_EVENT_CAPACITY || counts.revision!=counts.events)return UMI_STATUS_INVALID_STATE;
    UmiBankAuditReport *report=calloc(1,sizeof *report);if(report==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    report->summary.query=*query;report->summary.revision=counts.revision;report->summary.totalEvents=counts.events;report->summary.durable=counts.durable;
    /* Keep one immutable copy. Matching by accepted revision prevents an
     * approval from borrowing the later posting or reversal's journal. */
    for(size_t i=0;i<counts.events;++i){
        const UmiBankAuditEvent *event=&operations->state->events[i];
        if(event->revision!=i+1U || event->command.expectedRevision!=i || BankCommandValid(&event->actor,&event->command)!=UMI_STATUS_OK){status=UMI_STATUS_INVALID_STATE;break;}
        if(!BankAuditMatches(query,event))continue;
        size_t at=report->summary.count++;report->rows[at].event=*event;report->journalStart[at]=report->summary.journalCount;
        for(size_t j=0;j<counts.journals;++j){
            const UmiBankJournal *journal=&operations->state->journals[j];if(journal->revision!=event->revision)continue;
            status=AuditJournalValid(journal,event);if(status!=UMI_STATUS_OK)break;
            report->journals[report->summary.journalCount++]=*journal;++report->rows[at].journalCount;
        }
        if(status!=UMI_STATUS_OK)break;
    }
    /* Even a filtered capture must not silently hide orphaned stored journals. */
    for(size_t i=0;status==UMI_STATUS_OK && i<counts.journals;++i){
        const UmiBankJournal *journal=&operations->state->journals[i];
        if(!journal->revision || journal->revision>counts.revision)status=UMI_STATUS_INVALID_STATE;
        else status=AuditJournalValid(journal,&operations->state->events[journal->revision-1U]);
    }
    if(status!=UMI_STATUS_OK){free(report);return status;}
    *outReport=report;return UMI_STATUS_OK;
}
void UmiBankAuditDestroy(UmiBankAuditReport *report){free(report);}
UmiStatus UmiBankAuditReadSummary(const UmiBankAuditReport *report,UmiBankAuditSummary *out)
{if(report==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=report->summary;return UMI_STATUS_OK;}
UmiStatus UmiBankAuditRowAt(const UmiBankAuditReport *report,size_t index,UmiBankAuditRow *out)
{
    if(report==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=report->summary.count)return UMI_STATUS_NOT_FOUND;
    *out=report->rows[index];return UMI_STATUS_OK;
}
UmiStatus UmiBankAuditJournalAt(const UmiBankAuditReport *report,size_t eventIndex,size_t journalIndex,UmiBankJournal *out)
{
    if(report==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(eventIndex>=report->summary.count || journalIndex>=report->rows[eventIndex].journalCount)return UMI_STATUS_NOT_FOUND;
    *out=report->journals[report->journalStart[eventIndex]+journalIndex];return UMI_STATUS_OK;
}
