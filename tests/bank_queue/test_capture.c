/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/test_capture.c
 * PURPOSE: Verify queue ordering, filters, immutable ownership and lifecycle projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));QueueSetup(&f,250);
    UmiBankWorkQueue *queue=QueueCapture(&f);UmiBankWorkQueueSummary summary;OK(UmiBankWorkQueueSummaryRead(queue,&summary));
    CHECK(summary.count==3&&summary.pending==2&&summary.approved==1&&summary.totalOpen==3&&summary.revision==9&&!summary.durable);
    UmiBankWorkQueueRow row;UmiBankWorkQueueFilter filter=UmiBankWorkQueueFilterAll();
    if(strcmp(name,"ordered")==0){
        const UmiBankWorkKind kinds[]={UMI_BANK_WORK_INTEREST,UMI_BANK_WORK_CHARGE,UMI_BANK_WORK_TRANSFER};
        for(size_t i=0;i<3;++i){OK(UmiBankWorkQueueRowAt(queue,i,&row));CHECK(row.kind==kinds[i]&&row.submittedRevision==6+i&&strcmp(row.id.value,"shared")==0);}
        OK(UmiBankWorkQueueRowAt(queue,0,&row));CHECK(row.principal.minor_units==100000&&row.amount.minor_units==410&&row.interest.days==30);
        OK(UmiBankWorkQueueRowAt(queue,1,&row));CHECK(row.amount.minor_units==250&&strstr(row.reason,"review")!=NULL&&strcmp(row.checkerId.value,"checker")==0);
    }else if(strcmp(name,"kinds")==0||strcmp(name,"states")==0||strcmp(name,"destination")==0){
        if(strcmp(name,"kinds")==0)filter.kinds=UMI_BANK_WORK_INTEREST|UMI_BANK_WORK_TRANSFER;
        if(strcmp(name,"states")==0)filter.states=UMI_BANK_QUEUE_APPROVED;
        if(strcmp(name,"destination")==0)Id(&filter.accountId,"recipient");
        UmiBankWorkQueueDestroy(queue);queue=NULL;OK(UmiBankWorkQueueCapture(f.bank,&filter,&queue));OK(UmiBankWorkQueueSummaryRead(queue,&summary));
        CHECK(summary.totalOpen==3&&summary.count==(strcmp(name,"kinds")==0?2U:1U));
        OK(UmiBankWorkQueueRowAt(queue,0,&row));
        if(strcmp(name,"destination")==0)CHECK(row.kind==UMI_BANK_WORK_TRANSFER&&strcmp(row.destinationAccountId.value,"recipient")==0);
        if(strcmp(name,"states")==0)CHECK(row.kind==UMI_BANK_WORK_CHARGE&&summary.approved==1&&summary.pending==0);
    }else if(strcmp(name,"invalid")==0){
        UmiBankWorkQueue *invalid=(UmiBankWorkQueue *)(uintptr_t)1;filter.kinds=0;
        CHECK(UmiBankWorkQueueCapture(f.bank,&filter,&invalid)==UMI_STATUS_INVALID_ARGUMENT&&invalid==NULL);
        filter=UmiBankWorkQueueFilterAll();filter.states=8;CHECK(UmiBankWorkQueueCapture(f.bank,&filter,&invalid)==UMI_STATUS_INVALID_ARGUMENT);
        filter=UmiBankWorkQueueFilterAll();memset(filter.accountId.value,'a',sizeof(filter.accountId.value));CHECK(UmiBankWorkQueueCapture(f.bank,&filter,&invalid)==UMI_STATUS_INVALID_ARGUMENT);
        filter=UmiBankWorkQueueFilterAll();Id(&filter.accountId,"missing");CHECK(UmiBankWorkQueueCapture(f.bank,&filter,&invalid)==UMI_STATUS_NOT_FOUND&&invalid==NULL);
        CHECK(UmiBankWorkQueueCapture(NULL,NULL,&invalid)==UMI_STATUS_INVALID_ARGUMENT&&invalid==NULL);
    }else if(strcmp(name,"ownership")==0){
        OK(UmiBankWorkQueueRowAt(queue,0,&row));row.amount.minor_units=1;
        UmiBankOperationsDestroy(f.bank);f.bank=NULL;OK(UmiBankWorkQueueRowAt(queue,0,&row));CHECK(row.amount.minor_units==410);
        OK(UmiBankWorkQueueSummaryRead(queue,&summary));CHECK(summary.revision==9);
    }else if(strcmp(name,"closed")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_INTEREST_REJECT,"shared");Send(&f,&f.checker,&c);
        c=Make(&f,UMI_BANK_TRANSFER_CANCEL,"shared");Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_CHARGE_POST,"shared");Send(&f,&f.operator,&c);
        UmiBankWorkQueueDestroy(queue);queue=QueueCapture(&f);OK(UmiBankWorkQueueSummaryRead(queue,&summary));CHECK(summary.count==0&&summary.totalOpen==0);
    }else if(strcmp(name,"capacity")==0){
        for(unsigned i=1;i<UMI_BANK_RECORD_CAPACITY;++i){
            char id[40],reference[40];(void)snprintf(id,sizeof(id),"request-%u",i);(void)snprintf(reference,sizeof(reference),"reference-%u",i);
            UmiBankCommand c=Interest(&f,id,reference);Send(&f,&f.maker,&c);
            c=Charge(&f,id,reference);Send(&f,&f.maker,&c);
            c=Make(&f,UMI_BANK_TRANSFER_SUBMIT,id);Id(&c.ownerId,"beneficiary");Id(&c.sourceAccountId,"account");c.amount=Cash(1000);Send(&f,&f.maker,&c);
        }
        UmiBankWorkQueueDestroy(queue);queue=QueueCapture(&f);OK(UmiBankWorkQueueSummaryRead(queue,&summary));
        CHECK(summary.count==UMI_BANK_WORK_QUEUE_CAPACITY&&summary.pending==UMI_BANK_WORK_QUEUE_CAPACITY-1U&&summary.approved==1);
        OK(UmiBankWorkQueueRowAt(queue,UMI_BANK_WORK_QUEUE_CAPACITY-1U,&row));CHECK(row.kind==UMI_BANK_WORK_TRANSFER);
        Balance(&f,100000,64000);
    }else if(strcmp(name,"bounds")==0){
        memset(&row,0x5a,sizeof(row));CHECK(UmiBankWorkQueueRowAt(queue,3,&row)==UMI_STATUS_NOT_FOUND&&row.id.value[0]=='\0');
        UmiBankAction action=UMI_BANK_TEST_CREDIT;CHECK(UmiBankWorkQueueResolveAction(queue,0,UMI_BANK_WORK_POST,&action)==UMI_STATUS_INVALID_STATE&&action==UMI_BANK_TEST_CREDIT);
        CHECK(UmiBankWorkQueueResolveAction(queue,1,UMI_BANK_WORK_APPROVE,&action)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiBankWorkQueueResolveAction(queue,0,(UmiBankWorkDecision)9,&action)==UMI_STATUS_INVALID_ARGUMENT);
    }else CHECK(0);
    if(f.bank!=NULL)Balance(&f,strcmp(name,"closed")==0?99750:100000,strcmp(name,"closed")==0?0:strcmp(name,"capacity")==0?64000:1000);
    UmiBankWorkQueueDestroy(queue);UmiBankOperationsDestroy(f.bank);return 0;
}
