/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_audit/test_reports.c
 * PURPOSE: Verify readable details, complete command payloads, exact journals and atomic CSV output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f=AuditOpen();UmiBankAuditQuery q={0};q.family=UMI_BANK_AUDIT_CHARGE;
    if(strcmp(name,"empty")==0)Id(&q.entityId,"missing");
    if(strcmp(name,"invalid-utf8")==0){
        UmiBankCommand c=Make(&f,UMI_BANK_CHARGE_SUBMIT,"bad-text");Id(&c.sourceAccountId,"account");Id(&c.ownerId,"utf8");c.amount=Cash(1);c.name[0]=(char)0xff;c.name[1]=0;Send(&f,&f.maker,&c);
    }
    UmiBankAuditReport *report=AuditCapture(&f,q);size_t required=0;
    /* Optional size and text outputs are independent. Rejected reports clear
     * whichever outputs the caller supplied, without dereferencing the other. */
    char refused[8]="before";
    required=99;
    CHECK(UmiBankAuditDescribe(NULL,refused,sizeof refused,&required)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(required==0 && refused[0]=='\0');
    required=99;
    CHECK(UmiBankAuditDescribeEvent(NULL,0,NULL,0,&required)==UMI_STATUS_INVALID_ARGUMENT && required==0);
    memcpy(refused,"before",7);
    CHECK(UmiBankAuditDescribeEvent(NULL,0,refused,sizeof refused,NULL)==UMI_STATUS_INVALID_ARGUMENT && refused[0]=='\0');

    if(strcmp(name,"summary")==0 || strcmp(name,"empty")==0){
        OK(UmiBankAuditDescribe(report,NULL,0,&required));char *text=malloc(required);CHECK(text!=NULL);OK(UmiBankAuditDescribe(report,text,required,NULL));
        CHECK(strstr(text,"LOCAL PRACTICE AUDIT") && strstr(text,"Captured revision: 9"));
        CHECK(strstr(text,strcmp(name,"empty")==0?"No accepted commands match":"Matching accepted commands: 4 of 9"));free(text);
    }else if(strcmp(name,"detail")==0 || strcmp(name,"no-posting")==0){
        size_t at=strcmp(name,"detail")==0?2:0;OK(UmiBankAuditDescribeEvent(report,at,NULL,0,&required));char *text=malloc(required);CHECK(text!=NULL);
        OK(UmiBankAuditDescribeEvent(report,at,text,required,NULL));
        if(at==2)CHECK(strstr(text,"bank-journal-6") && strstr(text,"sys.charges.GBP") && strstr(text,"GBP 2.50") && !strstr(text,"bank-journal-7"));
        else CHECK(strstr(text,"This command created no booked journal") && strstr(text,"\\\"review\\\"") && strstr(text,"\\\\ note"));
        free(text);
    }else if(strcmp(name,"capacity")==0){
        char text[8]="before";OK(UmiBankAuditDescribeEvent(report,0,NULL,0,&required));size_t measured=required;
        CHECK(UmiBankAuditDescribeEvent(report,0,text,sizeof text,&required)==UMI_STATUS_CAPACITY_EXCEEDED && !text[0] && required==measured);
        CHECK(UmiBankAuditDescribeEvent(report,99,text,sizeof text,&required)==UMI_STATUS_NOT_FOUND && !text[0] && required==0);
    }else if(strcmp(name,"csv")==0 || strcmp(name,"journal-csv")==0 || strcmp(name,"invalid-utf8")==0){
        UmiCsvDocument *csv=(void *)1;UmiStatus status=strcmp(name,"journal-csv")==0?UmiBankAuditExportJournalsCsv(report,&csv):UmiBankAuditExportCsv(report,&csv);
        if(strcmp(name,"invalid-utf8")==0)CHECK(status!=UMI_STATUS_OK && csv==NULL);
        else {OK(status);CHECK(UmiCsvDocumentRows(csv)==6 && strstr(UmiCsvDocumentData(csv),"LOCAL PRACTICE"));
            if(strcmp(name,"csv")==0)CHECK(strstr(UmiCsvDocumentData(csv),"\"'=Fee, \"\"review\"\"") && !strstr(UmiCsvDocumentData(csv),"bank-journal-6"));
            else CHECK(strstr(UmiCsvDocumentData(csv),"bank-journal-6") && strstr(UmiCsvDocumentData(csv),"bank-journal-7") && !strstr(UmiCsvDocumentData(csv),"bank-journal-3"));
            UmiCsvDocumentDestroy(csv);}
    }else return 2;
    UmiBankAuditDestroy(report);UmiBankOperationsDestroy(f.bank);return 0;
}
