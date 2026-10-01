/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_capacity.c
 * PURPOSE: Exercise full hold/transfer capacity and exact amounts beyond floating-point precision.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include <limits.h>
int main(int argc,char **argv)
{
    CHECK(argc==2);Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);
    if(strcmp(argv[1],"full")==0){
        ReservationDestination(&f);
        for(size_t i=0;i<UMI_BANK_RECORD_CAPACITY;++i){
            char id[48];(void)snprintf(id,sizeof id,"reserve-%zu",i);
            UmiBankCommand c=Make(&f,UMI_BANK_HOLD_PLACE,id);Id(&c.sourceAccountId,"account");c.amount=Cash(1);Send(&f,&f.operator,&c);
            c=Make(&f,UMI_BANK_TRANSFER_SUBMIT,id);Id(&c.sourceAccountId,"account");Id(&c.ownerId,"beneficiary");c.amount=Cash(2);Send(&f,&f.maker,&c);
        }
        UmiBankReservations *report=CaptureReservations(&f);UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(report,&s));
        CHECK(s.count==UMI_BANK_RESERVATION_CAPACITY && s.manualMinor==64 && s.transferMinor==128 && s.balance.reserved.minor_units==192);
        UmiBankReservationRow row;OK(UmiBankReservationsRowAt(report,s.count-1U,&row));CHECK(row.kind==UMI_BANK_RESERVATION_TRANSFER);
        UmiBankReservationsDestroy(report);
    }else{
        CHECK(strcmp(argv[1],"large")==0);
        UmiBankCommand c=Make(&f,UMI_BANK_TEST_CREDIT,"large-funding");Id(&c.sourceAccountId,"account");c.amount=Cash(INT64_MAX-100000);Send(&f,&f.operator,&c);
        c=Make(&f,UMI_BANK_HOLD_PLACE,"large-hold");Id(&c.sourceAccountId,"account");c.amount=Cash(INT64_MAX);Send(&f,&f.operator,&c);
        UmiBankReservations *report=CaptureReservations(&f);UmiBankReservationsSummary s;OK(UmiBankReservationsReadSummary(report,&s));
        CHECK(s.manualMinor==INT64_MAX && s.balance.reserved.minor_units==INT64_MAX && s.balance.available.minor_units==0);
        UmiCsvDocument *csv=NULL;OK(UmiBankReservationsExportCsv(report,&csv));CHECK(strstr(UmiCsvDocumentData(csv),"9223372036854775807")!=NULL);
        UmiCsvDocumentDestroy(csv);UmiBankReservationsDestroy(report);
    }
    UmiBankOperationsDestroy(f.bank);return 0;
}
