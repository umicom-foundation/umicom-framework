/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_release.c
 * PURPOSE: Reject stale, foreign and unauthorised releases before reviewed publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);ReservationPopulate(&f);
    UmiBankReservations *report=CaptureReservations(&f);size_t index=strcmp(name,"card")==0?1U:
        (strcmp(name,"transfer")==0 || strcmp(name,"other-maker")==0)?2U:0U;
    UmiBankCommand c=ReleaseCommand(&f,report,index);UmiBankActor actor=f.operator;
    UmiStatus expected=UMI_STATUS_OK;UmiBankReview *review=NULL;
    if(strcmp(name,"permission")==0){actor.capabilities=0;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(strcmp(name,"other-maker")==0){actor=f.maker;Id(&actor.id,"other-maker");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(strcmp(name,"wrong-id")==0){Id(&c.id,"card-hold");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"wrong-action")==0){c.action=UMI_BANK_CARD_VOID;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"wrong-revision")==0){++c.expectedRevision;expected=UMI_STATUS_BUSY;}
    else if(strcmp(name,"stale")==0){
        UmiBankCommand changed=Make(&f,UMI_BANK_TEST_CREDIT,"later");Id(&changed.sourceAccountId,"account");changed.amount=Cash(1);Send(&f,&f.operator,&changed);expected=UMI_STATUS_BUSY;
    }else if(strcmp(name,"foreign")==0){
        Fixture other={0};OK(UmiBankOperationsOpenMemory(&other.bank));Setup(&other);Id(&other.operator.id,"foreign-operator");ReservationPopulate(&other);
        CHECK(UmiBankReservationsReviewRelease(other.bank,report,index,&actor,&c,&review)==UMI_STATUS_BUSY && review==NULL);
        UmiBankOperationsDestroy(other.bank);UmiBankOperationsDestroy(f.bank);UmiBankReservationsDestroy(report);return 0;
    }else CHECK(strcmp(name,"manual")==0 || strcmp(name,"card")==0 || strcmp(name,"transfer")==0 || strcmp(name,"execution-stale")==0);
    UmiBankCounts before,after;OK(UmiBankOperationsCounts(f.bank,&before));
    CHECK(UmiBankReservationsReviewRelease(f.bank,report,index,&actor,&c,&review)==expected);
    OK(UmiBankOperationsCounts(f.bank,&after));CHECK(before.revision==after.revision && before.journals==after.journals);
    if(expected==UMI_STATUS_OK){
        UmiBankReviewSnapshot *snapshot=calloc(1,sizeof *snapshot);CHECK(snapshot!=NULL);OK(UmiBankReviewSnapshotRead(review,snapshot));
        CHECK(snapshot->accountCount==1U && snapshot->accounts[0].before.booked.minor_units==snapshot->accounts[0].after.booked.minor_units);
        CHECK(snapshot->accounts[0].after.reserved.minor_units==6000-(index==0?1000:index==1?2000:3000));
        if(index<2U){CHECK(snapshot->hasHold && snapshot->holdExistedBefore && snapshot->holdBefore.state==UMI_BANK_HOLD_ACTIVE && snapshot->holdAfter.state==UMI_BANK_HOLD_RELEASED);}
        else CHECK(snapshot->hasTransfer && snapshot->transferAfter.state==UMI_BANK_TRANSFER_CANCELLED);
        free(snapshot);UmiBankReceipt receipt;
        if(strcmp(name,"execution-stale")==0){
            UmiBankCommand changed=Make(&f,UMI_BANK_TEST_CREDIT,"intervening");Id(&changed.sourceAccountId,"account");changed.amount=Cash(1);Send(&f,&f.operator,&changed);
            CHECK(UmiBankOperationsExecuteReviewed(f.bank,&actor,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0U);Balance(&f,100001,6000);
        }else{
            OK(UmiBankOperationsExecuteReviewed(f.bank,&actor,review,&receipt));Balance(&f,100000,6000-(index==0?1000:index==1?2000:3000));
            UmiBankReview *again=NULL;CHECK(UmiBankReservationsReviewRelease(f.bank,report,index,&actor,&c,&again)==UMI_STATUS_BUSY && again==NULL);
            UmiBankReservations *current=CaptureReservations(&f);UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(current,&s));CHECK(s.count==2U);UmiBankReservationsDestroy(current);
        }
    }else CHECK(review==NULL);
    UmiBankReviewDestroy(review);UmiBankReservationsDestroy(report);UmiBankOperationsDestroy(f.bank);return 0;
}
