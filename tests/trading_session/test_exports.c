/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_session/test_exports.c
 * PURPOSE: Check report buffer contracts, immutable CSV provenance and discrepancy exports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);ReviewFixture f;ReviewFixtureInit(&f);Fill(&f,f.first,"partial",1,100,1200);
    UmiTradingSessionSource *source=Source(&f);UmiTradingSessionReport *report=NULL;UmiCsvDocument *csv=NULL;
    bool issue=strcmp(argv[1],"issues")==0;
    if(issue)source->orders[0].filled_quantity=0;
    const char *filter=strcmp(argv[1],"empty")==0?"absent":strcmp(argv[1],"formula")==0?"=1+2":"";
    OK(UmiTradingBuildSessionReport(source,filter,&report));free(source);
    size_t required=0;OK(UmiTradingSessionReportDescribe(report,NULL,0,&required));CHECK(required>100);
    char *text=malloc(required);CHECK(text!=NULL);OK(UmiTradingSessionReportDescribe(report,text,required,NULL));CHECK(strlen(text)+1==required);
    char shortBuffer[4]="abc";size_t shortRequired=0;
    CHECK(UmiTradingSessionReportDescribe(report,shortBuffer,sizeof(shortBuffer),&shortRequired)==UMI_STATUS_CAPACITY_EXCEEDED&&shortBuffer[0]=='\0'&&shortRequired==required);
    OK(UmiTradingSessionReportExportCsv(report,&csv));const char *content=UmiCsvDocumentData(csv);
    CHECK(strstr(text,"not a broker statement")!=NULL&&strstr(content,"LOCAL RETAINED EVIDENCE")!=NULL);
    if(issue){CHECK(strstr(text,"Withheld:")!=NULL&&strstr(content,"totals-withheld")!=NULL&&strstr(content,"currency-total")==NULL);}
    else if(strcmp(argv[1],"empty")==0||strcmp(argv[1],"formula")==0){
        CHECK(UmiCsvDocumentRows(csv)==8&&strstr(content,f.first)==NULL);
        if(strcmp(argv[1],"formula")==0)CHECK(strstr(content,"\"'=1+2\"")!=NULL);
    }else if(strcmp(argv[1],"ownership")==0){
        UmiTradingSessionReportDestroy(report);report=NULL;umi_trading_workspace_destroy(f.workspace);f.workspace=NULL;
        CHECK(strstr(content,"partial")!=NULL&&strstr(content,"currency-total")!=NULL);
    }else if(strcmp(argv[1],"invalid")==0){
        UmiCsvDocument *bad=csv;CHECK(UmiTradingSessionReportExportCsv(NULL,&bad)==UMI_STATUS_INVALID_ARGUMENT&&bad==NULL);
        CHECK(UmiTradingSessionReportDescribe(report,NULL,1,&required)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTradingSessionReportDescribe(report,NULL,0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else CHECK(strcmp(argv[1],"text")==0);
    free(text);UmiCsvDocumentDestroy(csv);UmiTradingSessionReportDestroy(report);umi_trading_workspace_destroy(f.workspace);return 0;
}
