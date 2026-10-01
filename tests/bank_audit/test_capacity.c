/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/test_capacity.c
 * PURPOSE: Exercise a full accepted-event store and exact maximum-sized money without aggregate overflow.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));
    if(strcmp(name,"empty-book")==0){
        UmiBankAuditReport *report=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(report).revision==0 && AuditSummary(report).count==0);
        UmiCsvDocument *csv=NULL;OK(UmiBankAuditExportCsv(report,&csv));CHECK(UmiCsvDocumentRows(csv)==2);UmiCsvDocumentDestroy(csv);
        OK(UmiBankAuditExportJournalsCsv(report,&csv));CHECK(UmiCsvDocumentRows(csv)==2);UmiCsvDocumentDestroy(csv);
        UmiBankAuditDestroy(report);UmiBankOperationsDestroy(f.bank);return 0;
    }
    Setup(&f);
    if(strcmp(name,"full")==0){
        for(size_t i=3;i<UMI_BANK_EVENT_CAPACITY;++i){UmiBankCommand c=Make(&f,UMI_BANK_CUSTOMER_SET_STATE,"customer");c.state=i%2?UMI_BANK_RECORD_BLOCKED:UMI_BANK_RECORD_ACTIVE;Send(&f,&f.maker,&c);}
        UmiBankAuditReport *report=AuditCapture(&f,(UmiBankAuditQuery){0});CHECK(AuditSummary(report).count==UMI_BANK_EVENT_CAPACITY);
        UmiCsvDocument *csv=NULL;OK(UmiBankAuditExportCsv(report,&csv));CHECK(UmiCsvDocumentRows(csv)==UMI_BANK_EVENT_CAPACITY+2U);
        UmiCsvDocumentDestroy(csv);UmiBankAuditDestroy(report);
    }else if(strcmp(name,"large-money")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_ACCOUNT_OPEN,"large");Id(&c.ownerId,"customer");strcpy(c.name,"Large USD account");c.amount=Cash(0);memcpy(c.amount.currency.code,"USD",4);Send(&f,&f.maker,&c);
        c=Make(&f,UMI_BANK_TEST_CREDIT,"large-funding");Id(&c.sourceAccountId,"large");c.amount=Cash(INT64_MAX);memcpy(c.amount.currency.code,"USD",4);Send(&f,&f.operator,&c);
        UmiBankAuditReport *report=AuditCapture(&f,(UmiBankAuditQuery){.family=UMI_BANK_AUDIT_FUNDING});CHECK(AuditSummary(report).journalCount==2);
        UmiCsvDocument *csv=NULL;OK(UmiBankAuditExportJournalsCsv(report,&csv));CHECK(strstr(UmiCsvDocumentData(csv),"9223372036854775807") && strstr(UmiCsvDocumentData(csv),"USD") && strstr(UmiCsvDocumentData(csv),"GBP"));
        UmiCsvDocumentDestroy(csv);UmiBankAuditDestroy(report);
    }else return 2;
    UmiBankOperationsDestroy(f.bank);return 0;
}
