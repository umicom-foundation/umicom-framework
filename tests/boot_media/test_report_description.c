/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_report_description.c
 *
 * PURPOSE:
 *   Verify bounded media descriptions preserve destination storage and distinguish status from filesystem effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "umicom/boot_media/media.h"
#include <assert.h>
#include <string.h>
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    const char *cases[]={"empty", "created-failure", "verified-read", "verified-copy", "maximum-detail", "unicode", "maximum-progress", "exact-capacity", "short-capacity", "zero-capacity", "null-report", "null-output", "mismatched-status", "unknown-status", "negative-phase", "unknown-phase", "created-flag", "write-flag", "verified-flag", "verified-failure", "progress-overflow", "unterminated-detail", "invalid-utf8", "refusal-preserves-previous"};
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    UmiBootMediaReport report={0};
    strcpy(report.detail,"A final observation.");
    UmiStatus result=UMI_STATUS_OK,expected=UMI_STATUS_OK;
    if(!strcmp(mode,"empty"))report.detail[0]=0;
    if(!strcmp(mode,"created-failure")){
        result=report.status=UMI_STATUS_OUT_OF_MEMORY;report.outputCreated=1;
    }
    if(!strcmp(mode,"verified-read")){report.verified=1;report.phase=UMI_BOOT_MEDIA_VERIFY;}
    if(!strcmp(mode,"verified-copy")){
        report.outputCreated=report.writeStarted=report.verified=1;
        report.phase=UMI_BOOT_MEDIA_FINISHED;report.bytesDone=report.bytesTotal=512;
    }
    if(!strcmp(mode,"maximum-detail")){
        memset(report.detail,'d',sizeof report.detail-1U);report.detail[sizeof report.detail-1U]=0;
    }
    if(!strcmp(mode,"unicode"))strcpy(report.detail,"Practice \xCE\xA9 retained.");
    if(!strcmp(mode,"maximum-progress"))report.bytesDone=report.bytesTotal=UINT64_MAX;
    if(!strcmp(mode,"mismatched-status")){result=UMI_STATUS_IO_ERROR;expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"unknown-status")){result=report.status=(UmiStatus)999;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"negative-phase")){report.phase=(UmiBootMediaPhase)-1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"unknown-phase")){report.phase=(UmiBootMediaPhase)99;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"created-flag")){report.outputCreated=2;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"write-flag")){report.writeStarted=-1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"verified-flag")){report.verified=7;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"verified-failure")){
        result=report.status=UMI_STATUS_IO_ERROR;report.verified=1;expected=UMI_STATUS_INVALID_STATE;
    }
    if(!strcmp(mode,"progress-overflow")){report.bytesDone=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"unterminated-detail")){memset(report.detail,'x',sizeof report.detail);expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"invalid-utf8")){report.detail[0]=(char)0xff;report.detail[1]=0;expected=UMI_STATUS_INVALID_ARGUMENT;}
    char output[UMI_BOOT_MEDIA_REPORT_TEXT],before[sizeof output],full[sizeof output];
    memset(output,0x5a,sizeof output);
    size_t capacity=sizeof output;
    if(!strcmp(mode,"exact-capacity")||!strcmp(mode,"short-capacity")){
        assert(UmiBootMediaReportDescribe(result,&report,full,sizeof full)==UMI_STATUS_OK);
        capacity=strlen(full)+(!strcmp(mode,"exact-capacity")?1U:0U);
        if(!strcmp(mode,"short-capacity"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if(!strcmp(mode,"zero-capacity")){capacity=0;expected=UMI_STATUS_INVALID_ARGUMENT;}
    const UmiBootMediaReport *input=&report;
    char *destination=output;
    if(!strcmp(mode,"null-report")){input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"null-output")){destination=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    if(!strcmp(mode,"refusal-preserves-previous")){
        assert(UmiBootMediaReportDescribe(result,&report,output,sizeof output)==UMI_STATUS_OK);
        report.writeStarted=2;expected=UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(before,output,sizeof output);
    assert(UmiBootMediaReportDescribe(result,input,destination,capacity)==expected);
    if(expected!=UMI_STATUS_OK){assert(!memcmp(output,before,sizeof output));return 0;}
    assert(strstr(output,"Status: ")&&strstr(output,"Phase: ")&&
        strstr(output,"Practice file created: ")&&strstr(output,"Payload write attempted: ")&&
        strstr(output,"Byte agreement verified: ")&&strstr(output,"do not establish a successful boot"));
    assert(strstr(output,report.detail));
    assert(strstr(output,report.outputCreated?"Practice file created: yes":"Practice file created: no"));
    assert(strstr(output,report.verified?"Byte agreement verified: yes":"Byte agreement verified: no"));
    if(!strcmp(mode,"exact-capacity"))assert(!strcmp(output,full));
    if(!strcmp(mode,"maximum-progress"))assert(strstr(output,"18446744073709551615/18446744073709551615"));
    return 0;
}
