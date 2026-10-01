/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/audit_csv.c
 * PURPOSE: Export captured command evidence and journal lines with exact integer money and shared CSV escaping.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "audit_private.h"
enum AuditColumn {C_RECORD,C_SCOPE,C_CAPTURE,C_STORAGE,C_ACTOR_FILTER,C_ENTITY_FILTER,C_REQUEST_FILTER,C_FAMILY_FILTER,C_ACTION_FILTER,
    C_FIRST,C_LAST,C_FROM,C_TO,C_MATCHES,C_TOTAL,C_JOURNALS,C_REVISION,C_REQUEST,C_ACTOR,C_CAPS,C_FAMILY,C_ACTION_CODE,C_ACTION,C_ENTITY,
    C_DATE,C_TIME,C_EXPECTED,C_OWNER,C_SOURCE,C_DESTINATION,C_NAME,C_CURRENCY,C_SCALE,C_AMOUNT,C_STATE,C_RATE,C_DAYS,C_BASIS,
    C_JOURNAL_ID,C_REFERENCE,C_JOURNAL_DATE,C_JOURNAL_STATE,C_REVERSAL,C_LINE_ID,C_ACCOUNT,C_LINE_CURRENCY,C_LINE_SCALE,C_DEBIT,C_CREDIT,C_COLUMNS};
static const char *const AuditNames[C_COLUMNS]={"record","scope","captured_revision","storage","exact_actor","exact_entity","exact_request","workflow_filter","action_filter",
    "first_revision_inclusive","last_revision_inclusive","from_date_inclusive","to_date_inclusive","matched_events","total_events","matched_journals",
    "accepted_revision","request_id","actor_id","host_capabilities","workflow","action_code","action","entity_id","business_date","supplied_timestamp_millis",
    "expected_prior_revision","owner_reference","source_account","destination_account","stored_name_reason","command_currency","command_scale","command_minor",
    "requested_state","annual_rate_bps","interest_days","day_count_basis","journal_id","journal_reference","journal_date","journal_status","reversal",
    "line_id","line_account","line_currency","line_scale","debit_minor","credit_minor"};
static UmiStatus AuditCsv(const UmiBankAuditReport *report,int journals,UmiCsvDocument **outDocument)
{
    if(outDocument==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument=NULL;if(report==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *document=NULL;UmiStatus status=UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES,&document);if(status!=UMI_STATUS_OK)return status;
    UmiCsvCell cells[C_COLUMNS];for(size_t i=0;i<C_COLUMNS;++i)cells[i]=UmiCsvText(AuditNames[i]);
    status=UmiCsvDocumentAppendRow(document,cells,C_COLUMNS);
    for(size_t i=0;i<C_COLUMNS;++i)cells[i]=UmiCsvText("");
    const UmiBankAuditSummary *s=&report->summary;const UmiBankAuditQuery *q=&s->query;
    char from[11],to[11];BankActivityDateText(q->fromDate,from);BankActivityDateText(q->toDate,to);
    cells[C_RECORD]=UmiCsvText(journals?"journal-summary":"audit-summary");cells[C_SCOPE]=UmiCsvText("LOCAL PRACTICE");
    cells[C_CAPTURE]=UmiCsvUnsigned(s->revision);cells[C_STORAGE]=UmiCsvText(s->durable?"local durable store":"memory only");
    cells[C_ACTOR_FILTER]=UmiCsvText(q->actorId.value);cells[C_ENTITY_FILTER]=UmiCsvText(q->entityId.value);cells[C_REQUEST_FILTER]=UmiCsvText(q->requestId.value);
    cells[C_FAMILY_FILTER]=UmiCsvText(UmiBankAuditFamilyName(q->family));cells[C_ACTION_FILTER]=UmiCsvText(q->action?UmiBankActionName(q->action):"all");
    cells[C_FIRST]=UmiCsvUnsigned(q->firstRevision);cells[C_LAST]=UmiCsvUnsigned(q->lastRevision);cells[C_FROM]=UmiCsvText(from);cells[C_TO]=UmiCsvText(to);
    cells[C_MATCHES]=UmiCsvUnsigned(s->count);cells[C_TOTAL]=UmiCsvUnsigned(s->totalEvents);cells[C_JOURNALS]=UmiCsvUnsigned(s->journalCount);
    if(status==UMI_STATUS_OK)status=UmiCsvDocumentAppendRow(document,cells,C_COLUMNS);
    for(size_t i=0;status==UMI_STATUS_OK && i<s->count;++i){
        const UmiBankAuditRow *row=&report->rows[i];const UmiBankAuditEvent *e=&row->event;const UmiBankCommand *c=&e->command;
        for(size_t k=C_MATCHES;k<C_COLUMNS;++k)cells[k]=UmiCsvText("");
        char date[11];BankActivityDateText(c->businessDate,date);UmiBankAuditFamily family=UMI_BANK_AUDIT_ALL;(void)UmiBankAuditActionFamily(c->action,&family);
        cells[C_RECORD]=UmiCsvText(journals?"journal-line":"accepted-command");cells[C_REVISION]=UmiCsvUnsigned(e->revision);cells[C_REQUEST]=UmiCsvText(c->requestId.value);
        cells[C_ACTOR]=UmiCsvText(e->actor.id.value);cells[C_CAPS]=UmiCsvUnsigned(e->actor.capabilities);cells[C_FAMILY]=UmiCsvText(UmiBankAuditFamilyName(family));
        cells[C_ACTION_CODE]=UmiCsvUnsigned((unsigned)c->action);cells[C_ACTION]=UmiCsvText(UmiBankActionName(c->action));cells[C_ENTITY]=UmiCsvText(c->id.value);
        cells[C_DATE]=UmiCsvText(date);cells[C_TIME]=UmiCsvSigned(c->timestampMillis);cells[C_EXPECTED]=UmiCsvUnsigned(c->expectedRevision);
        cells[C_OWNER]=UmiCsvText(c->ownerId.value);cells[C_SOURCE]=UmiCsvText(c->sourceAccountId.value);cells[C_DESTINATION]=UmiCsvText(c->destinationAccountId.value);cells[C_NAME]=UmiCsvText(c->name);
        uint32_t fields=UmiBankActionFields(c->action);
        if(fields&UMI_BANK_FIELD_MONEY){cells[C_CURRENCY]=UmiCsvText(c->amount.currency.code);cells[C_SCALE]=UmiCsvUnsigned(c->amount.scale);cells[C_AMOUNT]=UmiCsvSigned(c->amount.minor_units);}
        if(fields&UMI_BANK_FIELD_STATE)cells[C_STATE]=UmiCsvUnsigned((unsigned)c->state);
        if(fields&UMI_BANK_FIELD_INTEREST){cells[C_RATE]=UmiCsvSigned(c->interest.annualRateBps);cells[C_DAYS]=UmiCsvUnsigned(c->interest.days);cells[C_BASIS]=UmiCsvUnsigned(c->interest.dayCountBasis);}
        if(!journals){status=UmiCsvDocumentAppendRow(document,cells,C_COLUMNS);continue;}
        for(size_t j=0;status==UMI_STATUS_OK && j<row->journalCount;++j){
            const UmiBankJournal *journal=&report->journals[report->journalStart[i]+j];char journalDate[11];BankActivityDateText(journal->entry.accounting_date,journalDate);
            cells[C_JOURNAL_ID]=UmiCsvText(journal->entry.id.value);cells[C_REFERENCE]=UmiCsvText(journal->referenceId.value);cells[C_JOURNAL_DATE]=UmiCsvText(journalDate);
            cells[C_JOURNAL_STATE]=UmiCsvUnsigned((unsigned)journal->entry.status);cells[C_REVERSAL]=UmiCsvText(journal->reversal?"yes":"no");
            cells[C_LINE_CURRENCY]=UmiCsvText(journal->currency.code);cells[C_LINE_SCALE]=UmiCsvUnsigned(journal->scale);
            for(size_t k=0;status==UMI_STATUS_OK && k<journal->entry.line_count;++k){
                const UmiAccountingJournalLine *line=&journal->entry.lines[k];cells[C_LINE_ID]=UmiCsvText(line->id.value);cells[C_ACCOUNT]=UmiCsvText(line->account_id.value);
                cells[C_DEBIT]=UmiCsvSigned(line->debit_minor);cells[C_CREDIT]=UmiCsvSigned(line->credit_minor);status=UmiCsvDocumentAppendRow(document,cells,C_COLUMNS);
            }
        }
    }
    if(status!=UMI_STATUS_OK){UmiCsvDocumentDestroy(document);return status;}
    *outDocument=document;return UMI_STATUS_OK;
}
UmiStatus UmiBankAuditExportCsv(const UmiBankAuditReport *report,UmiCsvDocument **outDocument)
{return AuditCsv(report,0,outDocument);}
UmiStatus UmiBankAuditExportJournalsCsv(const UmiBankAuditReport *report,UmiCsvDocument **outDocument)
{return AuditCsv(report,1,outDocument);}
