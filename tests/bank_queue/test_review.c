/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/test_review.c
 * PURPOSE: Verify typed request review, actor separation, stale histories and explicit execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));QueueSetup(&f,250);
    UmiBankWorkQueue *queue=QueueCapture(&f);UmiBankCommand c=QueueCommand(&f,queue,0,UMI_BANK_WORK_APPROVE);
    UmiBankReview *review=NULL;UmiBankReceipt receipt;UmiBankCounts before,after;OK(UmiBankOperationsCounts(f.bank,&before));
    if(strcmp(name,"approve")==0||strcmp(name,"execute")==0){
        OK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review));
        UmiBankReviewSnapshot *snapshot=calloc(1,sizeof(*snapshot));CHECK(snapshot!=NULL);OK(UmiBankReviewSnapshotRead(review,snapshot));
        CHECK(snapshot->hasInterest&&snapshot->interestAfter.amount.minor_units==410&&snapshot->interestAfter.state==UMI_BANK_TRANSFER_APPROVED);
        free(snapshot);OK(UmiBankOperationsCounts(f.bank,&after));CHECK(after.revision==before.revision);Balance(&f,100000,1000);
        if(strcmp(name,"execute")==0){OK(UmiBankOperationsExecuteReviewed(f.bank,&f.checker,review,&receipt));CHECK(receipt.revision==10);}
    }else if(strcmp(name,"post")==0){
        c=QueueCommand(&f,queue,1,UMI_BANK_WORK_POST);OK(UmiBankWorkQueueReview(f.bank,queue,1,&f.operator,&c,&review));
        Balance(&f,100000,1000);OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt));Balance(&f,99750,1000);
    }else if(strcmp(name,"same-maker")==0){
        UmiBankActor actor=f.maker;actor.capabilities|=UMI_BANK_CAP_APPROVE;
        CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&actor,&c,&review)==UMI_STATUS_PERMISSION_DENIED&&review==NULL);
    }else if(strcmp(name,"capabilities")==0){
        CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&f.operator,&c,&review)==UMI_STATUS_PERMISSION_DENIED&&review==NULL);
    }else if(strcmp(name,"wrong-kind")==0){
        CHECK(UmiBankWorkQueueReview(f.bank,queue,2,&f.checker,&c,&review)==UMI_STATUS_INVALID_ARGUMENT&&review==NULL);
    }else if(strcmp(name,"wrong-id")==0){
        Id(&c.id,"different");CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review)==UMI_STATUS_INVALID_ARGUMENT&&review==NULL);
    }else if(strcmp(name,"stale")==0){
        UmiBankCommand changed=Make(&f,UMI_BANK_CHARGE_CANCEL,"shared");Send(&f,&f.maker,&changed);
        CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review)==UMI_STATUS_BUSY&&review==NULL);
    }else if(strcmp(name,"history")==0||strcmp(name,"equivalent")==0){
        Fixture other={0};OK(UmiBankOperationsOpenMemory(&other.bank));QueueSetup(&other,strcmp(name,"history")==0?251:250);
        UmiStatus status=UmiBankWorkQueueReview(other.bank,queue,0,&f.checker,&c,&review);
        if(strcmp(name,"history")==0)CHECK(status==UMI_STATUS_BUSY&&review==NULL);else OK(status);
        UmiBankOperationsDestroy(other.bank);
    }else if(strcmp(name,"revision")==0){
        ++c.expectedRevision;CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review)==UMI_STATUS_BUSY&&review==NULL);
    }else CHECK(0);
    OK(UmiBankOperationsCounts(f.bank,&after));
    if(strcmp(name,"execute")!=0&&strcmp(name,"post")!=0&&strcmp(name,"stale")!=0)CHECK(after.revision==before.revision);
    UmiBankReviewDestroy(review);UmiBankWorkQueueDestroy(queue);UmiBankOperationsDestroy(f.bank);return 0;
}
