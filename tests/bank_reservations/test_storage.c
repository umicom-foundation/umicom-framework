/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_storage.c
 * PURPOSE: Keep captures immutable across reopen and reject publication after another writer commits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==3);const char *name=argv[1];Fixture f={0};FILE *file=fopen(argv[2],"wx");CHECK(file!=NULL && fclose(file)==0);
    UmiStatus status=UmiBankOperationsOpenSqlite(argv[2],&f.bank);
    if(status==UMI_STATUS_UNAVAILABLE){CHECK(remove(argv[2])==0);return 77;}OK(status);Setup(&f);ReservationPopulate(&f);
    UmiBankReservations *report=CaptureReservations(&f);UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(report,&s));CHECK(s.durable && s.count==3U);
    UmiBankCommand c=ReleaseCommand(&f,report,0U);UmiBankReview *review=NULL;
    if(strcmp(name,"reopen")==0){
        UmiBankOperationsDestroy(f.bank);f.bank=NULL;OK(UmiBankOperationsOpenSqlite(argv[2],&f.bank));
        OK(UmiBankReservationsReviewRelease(f.bank,report,0U,&f.operator,&c,&review));
        UmiBankReceipt receipt;OK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt));Balance(&f,100000,5000);
    }else{
        Fixture writer={0};writer.serial=100;writer.operator=f.operator;OK(UmiBankOperationsOpenSqlite(argv[2],&writer.bank));
        if(strcmp(name,"external-write")==0){
            OK(UmiBankReservationsReviewRelease(f.bank,report,0U,&f.operator,&c,&review));
            UmiBankCommand other=Make(&writer,UMI_BANK_TEST_CREDIT,"new-funds");Id(&other.sourceAccountId,"account");other.amount=Cash(1);Send(&writer,&writer.operator,&other);
            UmiBankReceipt receipt;CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.operator,review,&receipt)==UMI_STATUS_BUSY && receipt.revision==0U);
            Balance(&f,100000,6000);OK(UmiBankOperationsReload(f.bank));Balance(&f,100001,6000);
        }else{
            CHECK(strcmp(name,"reload")==0);
            UmiBankCommand other=Make(&writer,UMI_BANK_HOLD_RELEASE,"shared-id");Send(&writer,&writer.operator,&other);
            UmiBankReservations *cached=CaptureReservations(&f);OK(UmiBankReservationsReadSummary(cached,&s));CHECK(s.count==3U);UmiBankReservationsDestroy(cached);
            OK(UmiBankOperationsReload(f.bank));
            CHECK(UmiBankReservationsReviewRelease(f.bank,report,0U,&f.operator,&c,&review)==UMI_STATUS_BUSY && review==NULL);
            UmiBankReservations *current=CaptureReservations(&f);OK(UmiBankReservationsReadSummary(current,&s));CHECK(s.count==2U && s.balance.reserved.minor_units==5000);UmiBankReservationsDestroy(current);
        }
        UmiBankOperationsDestroy(writer.bank);
    }
    OK(UmiBankReservationsReadSummary(report,&s));CHECK(s.count==3U && s.balance.reserved.minor_units==6000);
    UmiBankReviewDestroy(review);UmiBankReservationsDestroy(report);UmiBankOperationsDestroy(f.bank);CHECK(remove(argv[2])==0);return 0;
}
