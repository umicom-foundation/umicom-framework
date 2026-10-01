/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_capture.c
 * PURPOSE: Check exact reservation explanations, typed identities, ordering and lifecycle exclusions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);
    if(strcmp(name,"empty")!=0)ReservationPopulate(&f);
    if(strcmp(name,"approved")==0){UmiBankCommand c=Make(&f,UMI_BANK_TRANSFER_APPROVE,"shared-id");Send(&f,&f.checker,&c);}
    if(strcmp(name,"released")==0){UmiBankCommand c=Make(&f,UMI_BANK_HOLD_RELEASE,"shared-id");Send(&f,&f.operator,&c);}
    if(strcmp(name,"voided")==0){UmiBankCommand c=Make(&f,UMI_BANK_CARD_VOID,"card-hold");Send(&f,&f.operator,&c);}
    if(strcmp(name,"captured")==0 || strcmp(name,"refunded")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_CARD_CAPTURE,"card-hold");c.amount=Cash(500);Send(&f,&f.operator,&c);
        if(strcmp(name,"refunded")==0){c=Make(&f,UMI_BANK_CARD_REFUND,"card-hold");Send(&f,&f.operator,&c);}
    }
    if(strcmp(name,"cancelled")==0){UmiBankCommand c=Make(&f,UMI_BANK_TRANSFER_CANCEL,"shared-id");Send(&f,&f.maker,&c);}
    if(strcmp(name,"executed")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_TRANSFER_APPROVE,"shared-id");Send(&f,&f.checker,&c);
        c=Make(&f,UMI_BANK_TRANSFER_EXECUTE,"shared-id");Send(&f,&f.operator,&c);
    }
    UmiBankCounts before,after;OK(UmiBankOperationsCounts(f.bank,&before));
    UmiBankReservations *report=NULL;
    if(strcmp(name,"unknown")==0 || strcmp(name,"invalid")==0){
        CHECK(UmiBankReservationsCapture(f.bank,strcmp(name,"unknown")==0?"missing":"bad account",&report)==
            (strcmp(name,"unknown")==0?UMI_STATUS_NOT_FOUND:UMI_STATUS_INVALID_ARGUMENT));CHECK(report==NULL);
    }else{
        const char *account=strcmp(name,"destination")==0?"destination":"account";
        OK(UmiBankReservationsCapture(f.bank,account,&report));UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(report,&s));
        CHECK(s.revision==before.revision && s.balance.revision==s.revision && !s.durable);
        CHECK(s.manualMinor+s.cardMinor+s.transferMinor==s.balance.reserved.minor_units);
        CHECK(s.balance.booked.minor_units-s.balance.reserved.minor_units==s.balance.available.minor_units);
        if(strcmp(name,"empty")==0 || strcmp(name,"destination")==0)CHECK(s.count==0U && s.balance.reserved.minor_units==0);
        else if(strcmp(name,"released")==0)CHECK(s.count==2U && s.manualMinor==0 && s.balance.reserved.minor_units==5000);
        else if(strcmp(name,"voided")==0 || strcmp(name,"captured")==0 || strcmp(name,"refunded")==0){
            CHECK(s.count==2U && s.cardMinor==0 && s.balance.reserved.minor_units==4000);
            CHECK(s.balance.booked.minor_units==(strcmp(name,"captured")==0?99500:100000));
        }else if(strcmp(name,"cancelled")==0 || strcmp(name,"executed")==0){
            CHECK(s.count==2U && s.transferMinor==0 && s.balance.reserved.minor_units==3000);
            CHECK(s.balance.booked.minor_units==(strcmp(name,"executed")==0?97000:100000));
        }else{
            CHECK(strcmp(name,"all")==0 || strcmp(name,"approved")==0 || strcmp(name,"ownership")==0 || strcmp(name,"bounds")==0);
            CHECK(s.count==3U && s.manualCount==1U && s.cardCount==1U && s.transferCount==1U);
            CHECK(s.manualMinor==1000 && s.cardMinor==2000 && s.transferMinor==3000 && s.balance.available.minor_units==94000);
            UmiBankReservationRow rows[3];for(size_t i=0;i<3U;++i)OK(UmiBankReservationsRowAt(report,i,&rows[i]));
            CHECK(rows[0].kind==UMI_BANK_RESERVATION_MANUAL && rows[1].kind==UMI_BANK_RESERVATION_CARD && rows[2].kind==UMI_BANK_RESERVATION_TRANSFER);
            CHECK(strcmp(rows[0].id.value,rows[2].id.value)==0 && rows[0].createdRevision<rows[1].createdRevision && rows[1].createdRevision<rows[2].createdRevision);
            CHECK(rows[2].transferState==(strcmp(name,"approved")==0?UMI_BANK_TRANSFER_APPROVED:UMI_BANK_TRANSFER_PENDING));
            CHECK(strcmp(rows[1].cardId.value,"card")==0 && strcmp(rows[2].destinationAccountId.value,"destination")==0);
            if(strcmp(name,"bounds")==0){
                UmiBankReservationRow output,old;memset(&output,0x5a,sizeof output);memcpy(&old,&output,sizeof old);
                CHECK(UmiBankReservationsRowAt(report,3U,&output)==UMI_STATUS_NOT_FOUND && memcmp(&old,&output,sizeof output)==0);
                UmiBankAction action=UMI_BANK_TEST_CREDIT;CHECK(UmiBankReservationsReleaseAction(report,SIZE_MAX,&action)==UMI_STATUS_NOT_FOUND && action==UMI_BANK_TEST_CREDIT);
            }
        }
    }
    OK(UmiBankOperationsCounts(f.bank,&after));CHECK(before.revision==after.revision && before.journals==after.journals);
    UmiBankOperationsDestroy(f.bank);
    if(strcmp(name,"ownership")==0){UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(report,&s));CHECK(s.count==3U && s.balance.available.minor_units==94000);}
    UmiBankReservationsDestroy(report);return 0;
}
