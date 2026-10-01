/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_reports.c
 * PURPOSE: Check exact captured report formatting, bounded output and lifetime-independent CSV.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);
    if(strcmp(name,"empty")!=0)ReservationPopulate(&f);
    UmiBankReservations *report=CaptureReservations(&f);
    if(strcmp(name,"immutable")==0){UmiBankCommand c=ReleaseCommand(&f,report,0U);Send(&f,&f.operator,&c);Balance(&f,100000,5000);}
    UmiBankOperationsDestroy(f.bank);size_t required=0U;OK(UmiBankReservationsDescribe(report,NULL,0U,&required));CHECK(required>0U);
    char *text=malloc(required);CHECK(text!=NULL);OK(UmiBankReservationsDescribe(report,text,required,NULL));
    if(strcmp(name,"empty")==0)CHECK(strstr(text,"No active reservations")!=NULL && strstr(text,"Available = booked minus reserved: GBP 1000.00")!=NULL);
    else CHECK(strstr(text,"Manual holds (1): GBP 10.00")!=NULL && strstr(text,"Card authorisations (1): GBP 20.00")!=NULL &&
        strstr(text,"Pending / approved transfers (1): GBP 30.00")!=NULL && strstr(text,"Available = booked minus reserved: GBP 940.00")!=NULL);
    if(strcmp(name,"capacity")==0){size_t actual=0U;CHECK(UmiBankReservationsDescribe(report,text,required-1U,&actual)==UMI_STATUS_CAPACITY_EXCEEDED && text[0]=='\0' && actual==required);}
    else CHECK(strcmp(name,"text")==0 || strcmp(name,"csv")==0 || strcmp(name,"immutable")==0 || strcmp(name,"empty")==0);
    UmiCsvDocument *csv=NULL;OK(UmiBankReservationsExportCsv(report,&csv));
    CHECK(UmiCsvDocumentRows(csv)==(strcmp(name,"empty")==0?2U:5U));
    CHECK(strstr(UmiCsvDocumentData(csv),"\"reservation-summary\"")!=NULL && strstr(UmiCsvDocumentData(csv),"\"LOCAL PRACTICE\"")!=NULL);
    if(strcmp(name,"empty")!=0)CHECK(strstr(UmiCsvDocumentData(csv),"\"94000\"")!=NULL && strstr(UmiCsvDocumentData(csv),"\"card-hold\"")!=NULL);
    UmiCsvDocumentDestroy(csv);free(text);UmiBankReservationsDestroy(report);return 0;
}
