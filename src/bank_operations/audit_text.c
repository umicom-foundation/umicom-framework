/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/audit_text.c
 * PURPOSE: Explain captured accepted commands, exact payloads and balanced journal lines.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "audit_private.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
typedef struct AuditText {char *text;size_t capacity,length;UmiStatus status;} AuditText;
static void AuditAppend(AuditText *sink,const char *format,...)
{
    if(sink->status!=UMI_STATUS_OK)return;
    size_t remaining=sink->length<sink->capacity?sink->capacity-sink->length:0;
    va_list args;va_start(args,format);
    int written=vsnprintf(remaining?sink->text+sink->length:NULL,remaining,format,args);va_end(args);
    if(written<0 || (size_t)written>SIZE_MAX-sink->length-1U)sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
    else sink->length+=(size_t)written;
}
/* Escape stored quotes, slashes and control bytes so payload text stays
 * distinct from report formatting; the copied public event remains exact. */
static void AuditQuoted(AuditText *sink,const char *text)
{
    AuditAppend(sink,"\"");
    for(const unsigned char *p=(const unsigned char *)text;*p;++p){
        if(*p=='\n')AuditAppend(sink,"\\n");else if(*p=='\r')AuditAppend(sink,"\\r");
        else if(*p=='\t')AuditAppend(sink,"\\t");else if(*p=='\\' || *p=='\"')AuditAppend(sink,"\\%c",*p);
        else if(*p<32 || *p==127)AuditAppend(sink,"\\x%02X",(unsigned)*p);else AuditAppend(sink,"%c",*p);
    }
    AuditAppend(sink,"\"");
}
static void AuditMoney(AuditText *sink,UmiMoney amount)
{
    char text[UMI_MONEY_TEXT_CAPACITY];UmiStatus status=UmiMoneyTextFormat(&amount,text,sizeof text,NULL);
    if(status!=UMI_STATUS_OK)sink->status=status;else AuditAppend(sink,"%s (minor units: %" PRId64 ", scale: %u)",text,amount.minor_units,(unsigned)amount.scale);
}
static void AuditContext(AuditText *sink,const UmiBankAuditSummary *s)
{
    const UmiBankAuditQuery *q=&s->query;char from[11],to[11];BankActivityDateText(q->fromDate,from);BankActivityDateText(q->toDate,to);
    AuditAppend(sink,"LOCAL PRACTICE AUDIT\nCaptured revision: %" PRIu64 "\nStorage: %s\nMatching accepted commands: %zu of %zu\nLinked journals: %zu\n"
        "Exact actor: %s\nExact entity: %s\nExact request: %s\nWorkflow: %s\nAction: %s\n"
        "Inclusive revisions: %" PRIu64 " to %" PRIu64 " (0 means unbounded)\nInclusive business dates: %s to %s\n\n",
        s->revision,s->durable?"persistent local SQLite":"memory only",s->count,s->totalEvents,s->journalCount,
        q->actorId.value[0]?q->actorId.value:"(any)",q->entityId.value[0]?q->entityId.value:"(any)",q->requestId.value[0]?q->requestId.value:"(any)",
        UmiBankAuditFamilyName(q->family),q->action?UmiBankActionName(q->action):"all",q->firstRevision,q->lastRevision,from[0]?from:"unbounded",to[0]?to:"unbounded");
}
static void AuditLimit(AuditText *sink)
{
    AuditAppend(sink,"\nAccepted local commands only. Failed attempts and repeated idempotent calls do not create events.\n"
        "Actor identity/capabilities were supplied by the trusted host; this report does not authenticate a person.\n"
        "Revision order is retained even when business dates are backdated. This captured report does not update automatically.\n");
}
static UmiStatus AuditFinish(AuditText *sink,size_t *required)
{
    if(sink->status==UMI_STATUS_OK && required!=NULL)*required=sink->length+1U;
    if(sink->status==UMI_STATUS_OK && sink->text!=NULL && sink->length>=sink->capacity)sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(sink->status!=UMI_STATUS_OK && sink->text!=NULL && sink->capacity)sink->text[0]='\0';
    return sink->status;
}
UmiStatus UmiBankAuditDescribe(const UmiBankAuditReport *report,char *output,size_t capacity,size_t *outRequired)
{
    /* Each optional output has its own validity rule. Keep these checks
     * independent so a size-only query and a text-only request both start
     * with defined output, even when the report is rejected below.
     * The compact spelling is retained for comparison with this correction. */
#if 0
    if(outRequired!=NULL)*outRequired=0;if(output!=NULL && capacity)output[0]='\0';
#endif
    if (outRequired != NULL) {
        *outRequired = 0;
    }
    if (output != NULL && capacity != 0U) {
        output[0] = '\0';
    }
    if(report==NULL || (output==NULL && capacity))return UMI_STATUS_INVALID_ARGUMENT;
    AuditText sink={output,capacity,0,UMI_STATUS_OK};AuditContext(&sink,&report->summary);
    if(!report->summary.count)AuditAppend(&sink,"No accepted commands match these filters.\n");
    for(size_t i=0;i<report->summary.count;++i){
        const UmiBankAuditRow *row=&report->rows[i];const UmiBankAuditEvent *e=&row->event;
        AuditAppend(&sink,"Revision %" PRIu64 " | %s | entity %s | request %s | actor %s | %zu journals\n",
            e->revision,UmiBankActionName(e->command.action),e->command.id.value,e->command.requestId.value,e->actor.id.value,row->journalCount);
    }
    AuditLimit(&sink);return AuditFinish(&sink,outRequired);
}
UmiStatus UmiBankAuditDescribeEvent(const UmiBankAuditReport *report,size_t index,char *output,size_t capacity,size_t *outRequired)
{
    /* Each optional output has its own validity rule. Keep these checks
     * independent so a size-only query and a text-only request both start
     * with defined output, even when the report is rejected below.
     * The compact spelling is retained for comparison with this correction. */
#if 0
    if(outRequired!=NULL)*outRequired=0;if(output!=NULL && capacity)output[0]='\0';
#endif
    if (outRequired != NULL) {
        *outRequired = 0;
    }
    if (output != NULL && capacity != 0U) {
        output[0] = '\0';
    }
    if(report==NULL || (output==NULL && capacity))return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=report->summary.count)return UMI_STATUS_NOT_FOUND;
    AuditText sink={output,capacity,0,UMI_STATUS_OK};AuditContext(&sink,&report->summary);
    const UmiBankAuditRow *row=&report->rows[index];const UmiBankAuditEvent *e=&row->event;const UmiBankCommand *c=&e->command;
    char date[11];BankActivityDateText(c->businessDate,date);
    AuditAppend(&sink,"ACCEPTED COMMAND\nRevision: %" PRIu64 "\nRequest: %s\nActor: %s\nHost capability mask: %" PRIu32 "\nAction: %s (%u)\nEntity: %s\n"
        "Business date: %s\nSupplied timestamp (milliseconds): %" PRId64 "\nExpected prior revision: %" PRIu64 "\nOwner / reference: %s\n"
        "Source account: %s\nDestination account: %s\nStored name / reason: ",e->revision,c->requestId.value,e->actor.id.value,e->actor.capabilities,
        UmiBankActionName(c->action),(unsigned)c->action,c->id.value,date,c->timestampMillis,c->expectedRevision,c->ownerId.value,c->sourceAccountId.value,c->destinationAccountId.value);
    AuditQuoted(&sink,c->name);AuditAppend(&sink,"\n");
    uint32_t fields=UmiBankActionFields(c->action);
    if(fields&UMI_BANK_FIELD_MONEY){AuditAppend(&sink,"Command amount: ");AuditMoney(&sink,c->amount);AuditAppend(&sink,"\n");}
    if(fields&UMI_BANK_FIELD_STATE)AuditAppend(&sink,"Requested state: %u (1 active, 2 blocked, 3 closed)\n",(unsigned)c->state);
    if(fields&UMI_BANK_FIELD_INTEREST)AuditAppend(&sink,"Interest rate basis points: %" PRId32 "\nInterest days: %" PRIu32 "\nDay-count basis: %" PRIu32 "\n",c->interest.annualRateBps,c->interest.days,c->interest.dayCountBasis);
    AuditAppend(&sink,"\nJOURNALS AT THIS EXACT ACCEPTED REVISION: %zu\n",row->journalCount);
    if(!row->journalCount)AuditAppend(&sink,"This command created no booked journal. Approval, reservation and lifecycle actions may change workflow without posting money.\n");
    for(size_t i=0;i<row->journalCount;++i){
        const UmiBankJournal *j=&report->journals[report->journalStart[index]+i];BankActivityDateText(j->entry.accounting_date,date);
        AuditAppend(&sink,"Journal: %s | reference: %s | date: %s | compensating reversal: %s\n",j->entry.id.value,j->referenceId.value,date,j->reversal?"yes":"no");
        for(size_t k=0;k<j->entry.line_count;++k){
            const UmiAccountingJournalLine *line=&j->entry.lines[k];UmiMoney money={0};money.currency=j->currency;money.scale=j->scale;
            AuditAppend(&sink,"  Line %s | account %s | debit ",line->id.value,line->account_id.value);money.minor_units=line->debit_minor;AuditMoney(&sink,money);
            AuditAppend(&sink," | credit ");money.minor_units=line->credit_minor;AuditMoney(&sink,money);AuditAppend(&sink,"\n");
        }
    }
    AuditLimit(&sink);return AuditFinish(&sink,outRequired);
}
