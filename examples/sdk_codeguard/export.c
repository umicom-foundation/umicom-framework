/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_codeguard/export.c
 *
 * PURPOSE:
 *   Demonstrate bounded new-file report export through an independently installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/codeguard.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>

static int ExportMain(int argc,char **argv)
{
    if(argc!=4 || (strcmp(argv[2],"text")!=0 && strcmp(argv[2],"json")!=0 && strcmp(argv[2],"sarif")!=0)){
        fputs("Usage: umicom-sdk-codeguard-export <workspace> text|json|sarif <new-absolute-file>\n",stderr);
        return 2;
    }
    UmiCodeGuardReportFormat format=strcmp(argv[2],"text")==0?UMI_CODEGUARD_REPORT_TEXT:
        strcmp(argv[2],"json")==0?UMI_CODEGUARD_REPORT_JSON:UMI_CODEGUARD_REPORT_SARIF;
    UmiCodeGuardService *service=NULL;
    UmiStatus status=umi_codeguard_service_create(argv[1],&service);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Cannot create review: %s\n",umi_status_text(status));return 2;}
    UmiCodeGuardProfile profile=umi_codeguard_profile_security();
    status=umi_codeguard_service_scan(service,&profile);
    if(status==UMI_STATUS_OK)
        status=umi_codeguard_report_file_new(argv[3],format,
            umi_codeguard_service_result(service),64U*1024U*1024U);
    UmiCodeGuardSummary summary=umi_codeguard_service_summary(service);
    int gate=umi_codeguard_summary_failed(&summary,profile.fail_on);
    /* Export borrows stable findings only for the synchronous call. A GUI
     * should transfer an owned immutable report to a worker before storage I/O. */
    umi_codeguard_service_destroy(service);
    if(status!=UMI_STATUS_OK){
        fprintf(stderr,"Review/export failed: %s. A new partial file may remain; existing files were not replaced.\n",umi_status_text(status));
        return 3;
    }
    if(puts("New report saved.")<0 || fflush(stdout)!=0 || ferror(stdout))return 3;
    return gate?1:0;
}
int main(int argc,char **argv)
{
    int result=2;
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,ExportMain,&result);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Cannot capture native arguments: %s\n",umi_status_text(status));return 2;}
    return result;
}
