/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/test_csv.c
 * PURPOSE: Verify copied queue reports retain exact values, quoted text and applied scope.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));QueueSetup(&f,250);
    UmiBankWorkQueueFilter filter=UmiBankWorkQueueFilterAll();
    if(strcmp(name,"filtered")==0){filter.kinds=UMI_BANK_WORK_CHARGE;Id(&filter.accountId,"account");}
    if(strcmp(name,"empty")==0){filter.kinds=UMI_BANK_WORK_INTEREST;filter.states=UMI_BANK_QUEUE_APPROVED;}
    UmiBankWorkQueue *queue=NULL;OK(UmiBankWorkQueueCapture(f.bank,&filter,&queue));
    if(strcmp(name,"ownership")==0){UmiBankOperationsDestroy(f.bank);f.bank=NULL;}
    UmiCsvDocument *document=NULL;OK(UmiBankWorkQueueExportCsv(queue,&document));
    UmiBankWorkQueueDestroy(queue);queue=NULL;const char *text=UmiCsvDocumentData(document);
    CHECK(strstr(text,"LOCAL PRACTICE")!=NULL&&strstr(text,"captured_revision")!=NULL&&strstr(text,"kind_mask")!=NULL);
    if(strcmp(name,"empty")==0)CHECK(UmiCsvDocumentRows(document)==2);
    else{
        CHECK(strstr(text,"\"250\"")!=NULL&&strstr(text,"Service, \"\"review\"\" caf\xc3\xa9")!=NULL);
        CHECK(UmiCsvDocumentRows(document)==(strcmp(name,"filtered")==0?3U:5U));
        if(strcmp(name,"filtered")==0)CHECK(strstr(text,"\"interest\"")==NULL&&strstr(text,"\"transfer\"")==NULL);
        else CHECK(strstr(text,"\"100000\"")!=NULL&&strstr(text,"\"410\"")!=NULL);
    }
    if(f.bank!=NULL){UmiBankCounts counts;OK(UmiBankOperationsCounts(f.bank,&counts));CHECK(counts.revision==9);}
    UmiCsvDocumentDestroy(document);UmiBankOperationsDestroy(f.bank);return 0;
}
