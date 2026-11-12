/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_cli_report.c
 *
 * PURPOSE:
 *   Verify native CLI report visibility and early-refusal status without acquiring files or devices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "../../src/boot_media/internal.h"
#include <assert.h>
static char captured[8192];
static size_t capturedBytes;
static unsigned deviceQueries;
static int refuseAllocation;
static int CapturePrint(FILE *stream,const char *format,...)
{
    assert(stream==stdout||stream==stderr);
    va_list arguments;va_start(arguments,format);
    int length=vsnprintf(captured+capturedBytes,sizeof captured-capturedBytes,format,arguments);
    va_end(arguments);
    assert(length>=0&&(size_t)length<sizeof captured-capturedBytes);
    capturedBytes+=(size_t)length;
    return length;
}
static UmiStatus RefuseDevice(const char *path,UmiBootMediaDevice *output)
{
    (void)path;(void)output;++deviceQueries;return UMI_STATUS_PERMISSION_DENIED;
}
static void *ReportCalloc(size_t count,size_t bytes)
{
    return refuseAllocation?NULL:calloc(count,bytes);
}
/* Exercise the real argument parser and Result boundary. Native device queries
 * are refused by a seam, and allocation refusal prevents discovery entirely. */
#define fprintf CapturePrint
#define BmDeviceCheck RefuseDevice
#define calloc ReportCalloc
#include "../../src/boot_media/cli.c"
#undef calloc
#undef BmDeviceCheck
#undef fprintf
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    const char *cases[]={"success", "created-failure", "unavailable", "mismatched-report", "malformed-report", "device-refusal", "allocation-refusal"};
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    int code,expected=0;
    if(!strcmp(mode,"device-refusal")){
        char *args[]={"umicom-media","review","--image","unused-image","--device","synthetic-device"};
        code=UmiBootMediaMain(6,args);expected=1;
        assert(deviceQueries==1U&&strstr(captured,"Device observation failed"));
        assert(strstr(captured,"Practice file created: no"));
    }else if(!strcmp(mode,"allocation-refusal")){
        char *args[]={"umicom-media","list"};refuseAllocation=1;
        code=UmiBootMediaMain(2,args);expected=1;
        assert(!deviceQueries&&strstr(captured,"inventory could not be allocated"));
    }else{
        UmiBootMediaReport report={0};UmiStatus status=UMI_STATUS_OK;
        strcpy(report.detail,"Synthetic observation.");
        if(!strcmp(mode,"created-failure")){
            status=report.status=UMI_STATUS_OUT_OF_MEMORY;report.outputCreated=1;expected=1;
        }
        if(!strcmp(mode,"unavailable")){status=report.status=UMI_STATUS_UNAVAILABLE;expected=77;}
        if(!strcmp(mode,"mismatched-report")){report.status=UMI_STATUS_IO_ERROR;expected=1;}
        if(!strcmp(mode,"malformed-report")){report.verified=2;expected=1;}
        code=Result(status,&report);
        if(!strcmp(mode,"created-failure"))assert(strstr(captured,"Practice file created: yes"));
        if(strstr(mode,"report"))assert(strstr(captured,"report unavailable"));
        else assert(strstr(captured,"Byte agreement verified: no"));
    }
    assert(code==expected&&capturedBytes>0U);
    return 0;
}
