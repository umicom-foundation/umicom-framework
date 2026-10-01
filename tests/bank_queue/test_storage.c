/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/test_storage.c
 * PURPOSE: Verify queue history across durable reopen and a concurrent writer using the real repository.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==3);const char *name=argv[1];Fixture f={0};
    FILE *created=fopen(argv[2],"wx");CHECK(created!=NULL&&fclose(created)==0);
    UmiStatus status=UmiBankOperationsOpenSqlite(argv[2],&f.bank);
    if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(argv[2])==0);return 77;}
    OK(status);QueueSetup(&f,250);UmiBankWorkQueue *queue=QueueCapture(&f);
    UmiBankCommand c=QueueCommand(&f,queue,0,UMI_BANK_WORK_APPROVE);UmiBankReview *review=NULL;UmiBankReceipt receipt;
    if(strcmp(name,"reopen")==0){
        UmiBankOperationsDestroy(f.bank);f.bank=NULL;OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank));
        OK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review));OK(UmiBankOperationsExecuteReviewed(f.bank,&f.checker,review,&receipt));
        UmiBankWorkQueueDestroy(queue);queue=QueueCapture(&f);UmiBankWorkQueueSummary summary;OK(UmiBankWorkQueueSummaryRead(queue,&summary));
        CHECK(summary.durable&&summary.approved==2&&summary.pending==1&&summary.revision==10);
    }else if(strcmp(name,"stale-writer")==0){
        Fixture other={0};OK(UmiBankOperationsOpenSqlite(argv[2],&other.bank));other.serial=100;other.maker=f.maker;
        UmiBankCommand changed=Make(&other,UMI_BANK_TRANSFER_CANCEL,"shared");Send(&other,&other.maker,&changed);
        /* Cached history still agrees. Execution must independently detect the
         * newer repository; no queue API silently reloads or retries it. */
        OK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review));
        CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.checker,review,&receipt)==UMI_STATUS_BUSY&&receipt.revision==0);
        UmiBankReviewDestroy(review);review=NULL;OK(UmiBankOperationsReload(f.bank));
        CHECK(UmiBankWorkQueueReview(f.bank,queue,0,&f.checker,&c,&review)==UMI_STATUS_BUSY&&review==NULL);
        UmiBankOperationsDestroy(other.bank);
    }else CHECK(0);
    UmiBankReviewDestroy(review);UmiBankWorkQueueDestroy(queue);UmiBankOperationsDestroy(f.bank);CHECK(remove(argv[2])==0);return 0;
}
