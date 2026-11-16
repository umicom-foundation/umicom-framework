/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_system/main.c
 *
 * PURPOSE:
 *   Demonstrate exact copied device observations and saved-connection drift through an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <umicom/media_generation/comfy_system.h>
#include <umicom/media_generation/comfy_inspection.h>
#include <umicom/platform/native_arguments.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* Keep the original no-argument ownership exercise while exposing the shared file command through the installed SDK example. The former implementation is retained for owner review. */
#if 0
int main(void)
#endif
static int SystemSample(void)
{
    UmiProviderConnection connection={0};
    strcpy(connection.id,"local-generation");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Reviewed local generation");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.enabled=true;connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    /* Synthetic bytes teach ownership without contacting or claiming to inspect
     * a real server. The caller must review transport and status in a live host. */
    char bytes[]="{\"system\":{\"ram_total\":17179869184,\"ram_free\":8589934592,"
        "\"embedded_python\":false},\"devices\":["
        "{\"name\":\"Synthetic CPU\",\"type\":\"cpu\",\"index\":null},"
        "{\"name\":\"Synthetic GPU\",\"type\":\"cuda\",\"index\":0,"
        "\"vram_total\":8589934592,\"vram_free\":4294967296,"
        "\"torch_vram_total\":2147483648,\"torch_vram_free\":1073741824}]}";
    UmiComfySystemStats *stats=NULL;
    UmiStatus status=UmiComfySystemStatsCreate(&connection,7U,bytes,sizeof bytes-1U,NULL,&stats);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Observation refused: %d\n",(int)status);return 1;}
    /* The response buffer can be reused after the successful copy. */
    memset(bytes,0,sizeof bytes);
    UmiComfySystemSummary summary;
    status=UmiComfySystemStatsInspect(stats,&summary);
    if(status!=UMI_STATUS_OK || summary.device_count!=2U || summary.ram_free!=UINT64_C(8589934592)){
        UmiComfySystemStatsDestroy(stats);return 1;
    }
    printf("Synthetic report: %zu devices, reported RAM free=%" PRIu64 " bytes\n",summary.device_count,summary.ram_free);
    for(size_t i=0U;i<summary.device_count;++i){
        UmiComfyDeviceObservation device;
        if(UmiComfySystemStatsDeviceAt(stats,i,&device)!=UMI_STATUS_OK){UmiComfySystemStatsDestroy(stats);return 1;}
        printf("%zu: %s (%s), index ",i,device.name,device.type);
        if((device.known&UMI_COMFY_DEVICE_INDEX)!=0U)printf("%" PRIu64 "\n",device.index);
        else puts("unknown");
        if(i==0U && (device.known&UMI_COMFY_DEVICE_INDEX)!=0U){UmiComfySystemStatsDestroy(stats);return 1;}
        if(i==1U && (device.known&UMI_COMFY_DEVICE_INDEX)==0U){UmiComfySystemStatsDestroy(stats);return 1;}
    }
    if(UmiComfySystemStatsCheckConnection(stats,&connection,7U)!=UMI_STATUS_OK){UmiComfySystemStatsDestroy(stats);return 1;}
    strcpy(connection.label,"Revised connection");
    if(UmiComfySystemStatsCheckConnection(stats,&connection,7U)!=UMI_STATUS_BUSY){UmiComfySystemStatsDestroy(stats);return 1;}
    UmiComfySystemStatsDestroy(stats);
    puts("Saved-setting drift detected. Reported free memory is not reserved model capacity.");
/* Flush the synthetic report before success so an output failure is visible to the caller. The former implementation is retained for owner review. */
#if 0
    return 0;
}
#endif
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}


static int SystemEntry(int argc,char **argv)
{
    if(argc==1)return SystemSample();
    if(argc>=2 && strcmp(argv[1],"comfy")==0)return UmiComfyInspectionCommand(argc-2,argv+2);
    fputs("Use no arguments for the synthetic sample, or comfy system --help.\n",stderr);return 2;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,SystemEntry,&result);
    return status==UMI_STATUS_OK?result:1;
}
