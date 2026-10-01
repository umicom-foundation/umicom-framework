/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/work_queue.c
 * PURPOSE: Project pending requests and prepare a review through the existing banking transition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "work_queue_private.h"
#include <stdlib.h>
#include <string.h>
UmiBankWorkQueueFilter UmiBankWorkQueueFilterAll(void)
{
    UmiBankWorkQueueFilter filter={0};filter.kinds=UMI_BANK_QUEUE_ALL_KINDS;filter.states=UMI_BANK_QUEUE_ALL_STATES;return filter;
}
const char *UmiBankWorkKindName(UmiBankWorkKind kind)
{
    switch(kind){case UMI_BANK_WORK_TRANSFER:return "transfer";case UMI_BANK_WORK_INTEREST:return "interest";
    case UMI_BANK_WORK_CHARGE:return "charge";default:return NULL;}
}
static void QueueAppend(UmiBankWorkQueue *queue,const UmiBankWorkQueueRow *row)
{
    uint32_t state=row->state==UMI_BANK_TRANSFER_PENDING?UMI_BANK_QUEUE_PENDING:
        row->state==UMI_BANK_TRANSFER_APPROVED?UMI_BANK_QUEUE_APPROVED:0;
    if(state==0)return;
    queue->summary.totalOpen++;
    const UmiBankWorkQueueFilter *filter=&queue->summary.filter;
    if((filter->kinds&(uint32_t)row->kind)==0||(filter->states&state)==0)return;
    if(filter->accountId.value[0]!='\0'&&strcmp(filter->accountId.value,row->sourceAccountId.value)!=0&&
        strcmp(filter->accountId.value,row->destinationAccountId.value)!=0)return;
    queue->rows[queue->summary.count++]=*row;
    if(state==UMI_BANK_QUEUE_PENDING)queue->summary.pending++;else queue->summary.approved++;
}
static int QueueOrder(const void *left,const void *right)
{
    const UmiBankWorkQueueRow *a=left,*b=right;
    if(a->submittedRevision!=b->submittedRevision)return a->submittedRevision<b->submittedRevision?-1:1;
    if(a->kind!=b->kind)return a->kind<b->kind?-1:1;
    return strcmp(a->id.value,b->id.value);
}
UmiStatus UmiBankWorkQueueCapture(const UmiBankOperations *operations,
    const UmiBankWorkQueueFilter *requested,UmiBankWorkQueue **outQueue)
{
    if(outQueue==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outQueue=NULL;
    UmiBankWorkQueueFilter filter=requested!=NULL?*requested:UmiBankWorkQueueFilterAll();
    if(filter.kinds==0||(filter.kinds&~UMI_BANK_QUEUE_ALL_KINDS)!=0||filter.states==0||
        (filter.states&~UMI_BANK_QUEUE_ALL_STATES)!=0||!BankIdValid(&filter.accountId,false))return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankCounts counts;UmiStatus status=UmiBankOperationsCounts(operations,&counts);
    if(status!=UMI_STATUS_OK)return status;
    if(counts.transfers>UMI_BANK_RECORD_CAPACITY||counts.interestRequests>UMI_BANK_RECORD_CAPACITY||
        counts.chargeRequests>UMI_BANK_RECORD_CAPACITY||counts.events>UMI_BANK_EVENT_CAPACITY)return UMI_STATUS_INVALID_STATE;
    if(filter.accountId.value[0]!='\0'&&BankFindAccount(operations->state,filter.accountId.value)<0)return UMI_STATUS_NOT_FOUND;
    UmiBankWorkQueue *queue=calloc(1,sizeof(*queue));if(queue==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    queue->summary.filter=filter;queue->summary.revision=counts.revision;queue->summary.durable=counts.durable;
    queue->eventCount=counts.events;
    if(counts.events!=0){
        queue->history=malloc(counts.events*sizeof(*queue->history));
        if(queue->history==NULL){free(queue);return UMI_STATUS_OUT_OF_MEMORY;}
        memcpy(queue->history,operations->state->events,counts.events*sizeof(*queue->history));
    }
    /* Three bounded registries share one projection; no second workflow state
     * machine or financial calculation is introduced by the queue. */
    for(size_t i=0;i<counts.transfers;++i){
        const UmiBankTransfer *item=&operations->state->transfers[i];UmiBankWorkQueueRow row={0};
        row.kind=UMI_BANK_WORK_TRANSFER;row.id=item->id;row.state=item->state;row.amount=item->amount;
        row.sourceAccountId=item->sourceAccountId;row.destinationAccountId=item->destinationAccountId;
        row.referenceId=item->beneficiaryId;row.makerId=item->makerId;row.checkerId=item->checkerId;
        row.submittedRevision=item->submittedRevision;QueueAppend(queue,&row);
    }
    for(size_t i=0;i<counts.interestRequests;++i){
        const UmiBankInterestRequest *item=&operations->state->interestRequests[i];UmiBankWorkQueueRow row={0};
        row.kind=UMI_BANK_WORK_INTEREST;row.id=item->id;row.state=item->state;row.amount=item->amount;
        row.sourceAccountId=item->accountId;row.referenceId=item->periodId;row.makerId=item->makerId;row.checkerId=item->checkerId;
        row.principal=item->principal;row.interest=item->terms;row.submittedRevision=item->submittedRevision;QueueAppend(queue,&row);
    }
    for(size_t i=0;i<counts.chargeRequests;++i){
        const UmiBankChargeRequest *item=&operations->state->chargeRequests[i];UmiBankWorkQueueRow row={0};
        row.kind=UMI_BANK_WORK_CHARGE;row.id=item->id;row.state=item->state;row.amount=item->amount;
        row.sourceAccountId=item->accountId;row.referenceId=item->referenceId;row.makerId=item->makerId;row.checkerId=item->checkerId;
        memcpy(row.reason,item->reason,sizeof(row.reason));row.submittedRevision=item->submittedRevision;QueueAppend(queue,&row);
    }
    qsort(queue->rows,queue->summary.count,sizeof(queue->rows[0]),QueueOrder);
    *outQueue=queue;return UMI_STATUS_OK;
}
void UmiBankWorkQueueDestroy(UmiBankWorkQueue *queue)
{
    if(queue==NULL)return;
    free(queue->history);free(queue);
}
UmiStatus UmiBankWorkQueueSummaryRead(const UmiBankWorkQueue *queue,UmiBankWorkQueueSummary *out)
{
    if(out!=NULL)memset(out,0,sizeof(*out));
    if(queue==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=queue->summary;return UMI_STATUS_OK;
}
UmiStatus UmiBankWorkQueueRowAt(const UmiBankWorkQueue *queue,size_t index,UmiBankWorkQueueRow *out)
{
    if(out!=NULL)memset(out,0,sizeof(*out));
    if(queue==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=queue->summary.count)return UMI_STATUS_NOT_FOUND;
    *out=queue->rows[index];return UMI_STATUS_OK;
}
UmiStatus UmiBankWorkQueueResolveAction(const UmiBankWorkQueue *queue,size_t index,
    UmiBankWorkDecision decision,UmiBankAction *outAction)
{
    if(outAction==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankWorkQueueRow row;UmiStatus status=UmiBankWorkQueueRowAt(queue,index,&row);
    if(status!=UMI_STATUS_OK)return status;
    if(decision<UMI_BANK_WORK_APPROVE||decision>UMI_BANK_WORK_POST)return UMI_STATUS_INVALID_ARGUMENT;
    if((row.state==UMI_BANK_TRANSFER_PENDING&&decision==UMI_BANK_WORK_POST)||
        (row.state==UMI_BANK_TRANSFER_APPROVED&&(decision==UMI_BANK_WORK_APPROVE||decision==UMI_BANK_WORK_REJECT)))
        return UMI_STATUS_INVALID_STATE;
    static const UmiBankAction transfer[]={UMI_BANK_TRANSFER_APPROVE,UMI_BANK_TRANSFER_REJECT,UMI_BANK_TRANSFER_CANCEL,UMI_BANK_TRANSFER_EXECUTE};
    static const UmiBankAction interest[]={UMI_BANK_INTEREST_APPROVE,UMI_BANK_INTEREST_REJECT,UMI_BANK_INTEREST_CANCEL,UMI_BANK_INTEREST_POST};
    static const UmiBankAction charge[]={UMI_BANK_CHARGE_APPROVE,UMI_BANK_CHARGE_REJECT,UMI_BANK_CHARGE_CANCEL,UMI_BANK_CHARGE_POST};
    const UmiBankAction *actions=row.kind==UMI_BANK_WORK_TRANSFER?transfer:row.kind==UMI_BANK_WORK_INTEREST?interest:charge;
    *outAction=actions[(unsigned)decision-1U];return UMI_STATUS_OK;
}
UmiStatus UmiBankWorkQueueReview(const UmiBankOperations *operations,const UmiBankWorkQueue *queue,
    size_t index,const UmiBankActor *actor,const UmiBankCommand *command,UmiBankReview **outReview)
{
    if(outReview==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outReview=NULL;
    if(operations==NULL||queue==NULL||command==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=BankCommandValid(actor,command);if(status!=UMI_STATUS_OK)return status;
    UmiBankWorkQueueRow row;status=UmiBankWorkQueueRowAt(queue,index,&row);if(status!=UMI_STATUS_OK)return status;
    bool allowed=false;
    for(int decision=UMI_BANK_WORK_APPROVE;decision<=UMI_BANK_WORK_POST;++decision){
        UmiBankAction action;
        if(UmiBankWorkQueueResolveAction(queue,index,(UmiBankWorkDecision)decision,&action)==UMI_STATUS_OK&&action==command->action)allowed=true;
    }
    if(!allowed||strcmp(command->id.value,row.id.value)!=0)return UMI_STATUS_INVALID_ARGUMENT;
    if(command->expectedRevision!=queue->summary.revision)return UMI_STATUS_BUSY;
    if(operations->poisoned||operations->state==NULL)return UMI_STATUS_INVALID_STATE;
    status=BankHistoryMatches(operations->state,queue->summary.revision,queue->history,queue->eventCount);
    if(status!=UMI_STATUS_OK)return status;
    return UmiBankOperationsReview(operations,actor,command,outReview);
}
