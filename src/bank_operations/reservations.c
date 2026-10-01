/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/reservations.c
 * PURPOSE: Project active canonical reservations and reconcile their exact totals to the account balance.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "reservations_private.h"
#include "internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
const char *UmiBankReservationKindName(UmiBankReservationKind kind)
{
    switch(kind){
    case UMI_BANK_RESERVATION_MANUAL:return "manual hold";
    case UMI_BANK_RESERVATION_CARD:return "card authorisation";
    case UMI_BANK_RESERVATION_TRANSFER:return "transfer";
    default:return NULL;
    }
}
static UmiStatus ReservationAppend(UmiBankReservations *report, UmiBankReservationRow row)
{
    UmiBankReservationsSummary *s=&report->summary;
    if(s->count>=UMI_BANK_RESERVATION_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(row.amount.minor_units<=0 || row.amount.scale!=s->balance.booked.scale ||
        !umi_accounting_currency_equal(row.amount.currency,s->balance.booked.currency))return UMI_STATUS_INVALID_STATE;
    UmiBankAction origin=row.kind==UMI_BANK_RESERVATION_MANUAL?UMI_BANK_HOLD_PLACE:
        row.kind==UMI_BANK_RESERVATION_CARD?UMI_BANK_CARD_AUTHORISE:UMI_BANK_TRANSFER_SUBMIT;
    const UmiBankAuditEvent *created=NULL;
    for(size_t i=0;i<report->eventCount;++i){
        const UmiBankAuditEvent *event=&report->history[i];
        if(event->command.action==origin && strcmp(event->command.id.value,row.id.value)==0){
            if(created!=NULL)return UMI_STATUS_INVALID_STATE;
            created=event;
        }
    }
    if(created==NULL || created->revision==0 || created->revision>s->revision ||
        created->command.amount.minor_units!=row.amount.minor_units ||
        created->command.amount.scale!=row.amount.scale ||
        !umi_accounting_currency_equal(created->command.amount.currency,row.amount.currency))return UMI_STATUS_INVALID_STATE;
    if(row.kind==UMI_BANK_RESERVATION_TRANSFER && row.createdRevision!=created->revision)return UMI_STATUS_INVALID_STATE;
    row.createdRevision=created->revision;row.makerId=created->actor.id;
    int64_t *total=row.kind==UMI_BANK_RESERVATION_MANUAL?&s->manualMinor:
        row.kind==UMI_BANK_RESERVATION_CARD?&s->cardMinor:&s->transferMinor;
    if(*total>INT64_MAX-row.amount.minor_units)return UMI_STATUS_CAPACITY_EXCEEDED;
    *total+=row.amount.minor_units;
    if(row.kind==UMI_BANK_RESERVATION_MANUAL)++s->manualCount;
    else if(row.kind==UMI_BANK_RESERVATION_CARD)++s->cardCount;
    else ++s->transferCount;
    report->rows[s->count++]=row;return UMI_STATUS_OK;
}
static int ReservationOrder(const void *left,const void *right)
{
    const UmiBankReservationRow *a=left,*b=right;
    if(a->createdRevision!=b->createdRevision)return a->createdRevision<b->createdRevision?-1:1;
    if(a->kind!=b->kind)return a->kind<b->kind?-1:1;
    return strcmp(a->id.value,b->id.value);
}
UmiStatus UmiBankReservationsCapture(const UmiBankOperations *operations,
    const char *accountId,UmiBankReservations **outReport)
{
    if(outReport==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=NULL;UmiFinancialId id={0};
    if(accountId==NULL || umi_financial_id_assign(&id,accountId)!=UMI_STATUS_OK || !BankIdValid(&id,true))return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankCounts counts;UmiStatus status=UmiBankOperationsCounts(operations,&counts);
    if(status!=UMI_STATUS_OK)return status;
    if(counts.holds>UMI_BANK_RECORD_CAPACITY || counts.transfers>UMI_BANK_RECORD_CAPACITY ||
        counts.events>UMI_BANK_EVENT_CAPACITY || counts.revision!=counts.events)return UMI_STATUS_INVALID_STATE;
    UmiBankBalance balance;status=UmiBankOperationsBalance(operations,accountId,&balance);
    if(status!=UMI_STATUS_OK)return status;
    if(balance.revision!=counts.revision)return UMI_STATUS_INVALID_STATE;
    UmiBankReservations *report=calloc(1,sizeof *report);if(report==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    report->summary.accountId=id;report->summary.balance=balance;report->summary.revision=counts.revision;report->summary.durable=counts.durable;
    report->eventCount=counts.events;
    if(counts.events!=0){
        report->history=malloc(counts.events*sizeof *report->history);
        if(report->history==NULL){free(report);return UMI_STATUS_OUT_OF_MEMORY;}
        memcpy(report->history,operations->state->events,counts.events*sizeof *report->history);
    }
    /* This is explanatory evidence, not another balance owner. Only active
     * holds and pending/approved outgoing transfers contribute to reservations. */
    for(size_t i=0;status==UMI_STATUS_OK && i<counts.holds;++i){
        const UmiBankHold *hold=&operations->state->holds[i];
        if(hold->state!=UMI_BANK_HOLD_ACTIVE || strcmp(hold->accountId.value,accountId)!=0)continue;
        UmiBankReservationRow row={0};row.kind=hold->cardId.value[0]?UMI_BANK_RESERVATION_CARD:UMI_BANK_RESERVATION_MANUAL;
        row.id=hold->id;row.accountId=hold->accountId;row.cardId=hold->cardId;row.amount=hold->amount;
        status=ReservationAppend(report,row);
    }
    for(size_t i=0;status==UMI_STATUS_OK && i<counts.transfers;++i){
        const UmiBankTransfer *transfer=&operations->state->transfers[i];
        if((transfer->state!=UMI_BANK_TRANSFER_PENDING && transfer->state!=UMI_BANK_TRANSFER_APPROVED) ||
            strcmp(transfer->sourceAccountId.value,accountId)!=0)continue;
        UmiBankReservationRow row={0};row.kind=UMI_BANK_RESERVATION_TRANSFER;
        row.id=transfer->id;row.accountId=transfer->sourceAccountId;row.destinationAccountId=transfer->destinationAccountId;
        row.referenceId=transfer->beneficiaryId;row.amount=transfer->amount;row.transferState=transfer->state;row.createdRevision=transfer->submittedRevision;
        status=ReservationAppend(report,row);
    }
    UmiBankReservationsSummary *s=&report->summary;
    if(status==UMI_STATUS_OK && (s->manualMinor>INT64_MAX-s->cardMinor ||
        s->manualMinor+s->cardMinor>INT64_MAX-s->transferMinor))status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(status==UMI_STATUS_OK && s->manualMinor+s->cardMinor+s->transferMinor!=balance.reserved.minor_units)status=UMI_STATUS_INVALID_STATE;
    if(status!=UMI_STATUS_OK){UmiBankReservationsDestroy(report);return status;}
    qsort(report->rows,s->count,sizeof report->rows[0],ReservationOrder);
    *outReport=report;return UMI_STATUS_OK;
}
void UmiBankReservationsDestroy(UmiBankReservations *report)
{if(report!=NULL){free(report->history);free(report);}}
UmiStatus UmiBankReservationsReadSummary(const UmiBankReservations *report,UmiBankReservationsSummary *out)
{if(report==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=report->summary;return UMI_STATUS_OK;}
UmiStatus UmiBankReservationsRowAt(const UmiBankReservations *report,size_t index,UmiBankReservationRow *out)
{
    if(report==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=report->summary.count)return UMI_STATUS_NOT_FOUND;
    *out=report->rows[index];return UMI_STATUS_OK;
}
UmiStatus UmiBankReservationsReleaseAction(const UmiBankReservations *report,size_t index,UmiBankAction *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankReservationRow row;UmiStatus status=UmiBankReservationsRowAt(report,index,&row);
    if(status!=UMI_STATUS_OK)return status;
    switch(row.kind){
    case UMI_BANK_RESERVATION_MANUAL:*out=UMI_BANK_HOLD_RELEASE;break;
    case UMI_BANK_RESERVATION_CARD:*out=UMI_BANK_CARD_VOID;break;
    case UMI_BANK_RESERVATION_TRANSFER:*out=UMI_BANK_TRANSFER_CANCEL;break;
    default:return UMI_STATUS_INVALID_STATE;
    }return UMI_STATUS_OK;
}
UmiStatus UmiBankReservationsReviewRelease(const UmiBankOperations *operations,
    const UmiBankReservations *report,size_t index,const UmiBankActor *actor,
    const UmiBankCommand *command,UmiBankReview **outReview)
{
    if(outReview==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReview=NULL;
    if(operations==NULL || report==NULL || command==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=BankCommandValid(actor,command);if(status!=UMI_STATUS_OK)return status;
    UmiBankAction action;status=UmiBankReservationsReleaseAction(report,index,&action);if(status!=UMI_STATUS_OK)return status;
    if(command->action!=action || strcmp(command->id.value,report->rows[index].id.value)!=0)return UMI_STATUS_INVALID_ARGUMENT;
    if(command->expectedRevision!=report->summary.revision)return UMI_STATUS_BUSY;
    if(operations->poisoned || operations->state==NULL)return UMI_STATUS_INVALID_STATE;
    status=BankHistoryMatches(operations->state,report->summary.revision,report->history,report->eventCount);
    if(status!=UMI_STATUS_OK)return status;
    return UmiBankOperationsReview(operations,actor,command,outReview);
}
