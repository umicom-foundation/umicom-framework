/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_system_file.c
 *
 * PURPOSE:
 *   Verify composed file admission, output ownership, cancellation and copied system identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/media_generation/comfy_system_file.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int SystemFileTest(int argc,char **argv)
{
    CHECK(argc==4);const char *mode=argv[1],*document=argv[2],*input=argv[3];
    UmiComfySystemStats *stats=NULL;UmiStatus expected=UMI_STATUS_OK;
    UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"disabled"))expected=UMI_STATUS_PERMISSION_DENIED;
    else if(!strcmp(mode,"malformed-document") || !strcmp(mode,"malformed-system"))expected=UMI_STATUS_PARSE_ERROR;
    else if(!strcmp(mode,"missing-file"))expected=UMI_STATUS_NOT_FOUND;
    else if(!strcmp(mode,"directory"))expected=UMI_STATUS_IO_ERROR;
    else if(!strcmp(mode,"system-limit") || !strcmp(mode,"document-limit"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    else if(!strcmp(mode,"empty-file"))expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(!strcmp(mode,"relative-before-read")){input="relative.json";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-document")){document=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-system")){input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-output"))expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(!strcmp(mode,"cancel")){
        CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;
    }else if(!strcmp(mode,"occupied")){
        CHECK(UmiComfySystemStatsFileRead(document,input,NULL,&stats)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_ARGUMENT;
    }else CHECK(!strcmp(mode,"complete") || !strcmp(mode,"empty") || !strcmp(mode,"unicode-path"));
    UmiComfySystemStats *before=stats;
    UmiStatus status=UmiComfySystemStatsFileRead(document,input,cancel,!strcmp(mode,"null-output")?NULL:&stats);
    umi_cancellation_token_destroy(cancel);CHECK(status==expected);
    if(status==UMI_STATUS_OK){
        UmiComfySystemSummary summary;CHECK(UmiComfySystemStatsInspect(stats,&summary)==UMI_STATUS_OK);
        CHECK(summary.device_count==(!strcmp(mode,"empty")?0U:2U) && summary.connection_revision==7U);
        if(strcmp(mode,"empty")){
            CHECK(summary.ram_free==UINT64_C(9007199254740993));
            UmiComfyDeviceObservation device;CHECK(UmiComfySystemStatsDeviceAt(stats,0U,&device)==UMI_STATUS_OK);
            CHECK((device.known&UMI_COMFY_DEVICE_INDEX)==0U);
            CHECK(UmiComfySystemStatsDeviceAt(stats,1U,&device)==UMI_STATUS_OK && device.vram_inconsistent);
        }
    }else CHECK(stats==before);
    UmiComfySystemStatsDestroy(stats);return 0;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,SystemFileTest,&result);
    return status==UMI_STATUS_OK?result:1;
}
