/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_system_json.c
 *
 * PURPOSE:
 *   Serialize copied system observations with explicit unknowns and exact counters before publishing caller output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/media_generation/comfy_system_file.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void SystemKey(UmiJsonWriter *writer,const char *key)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);UmiJsonWriterRaw(writer,":");
}
static void SystemText(UmiJsonWriter *writer,const char *key,const char *value,bool known)
{
    SystemKey(writer,key);
    if(known)UmiJsonWriterString(writer,value);else UmiJsonWriterRaw(writer,"null");
}
static void SystemCount(UmiJsonWriter *writer,const char *key,size_t value)
{
    SystemKey(writer,key);UmiJsonWriterUInt64(writer,(uint64_t)value);
}
static void SystemBoolean(UmiJsonWriter *writer,const char *key,bool value,bool known)
{
    SystemKey(writer,key);
    if(known)UmiJsonWriterBool(writer,value);else UmiJsonWriterRaw(writer,"null");
}
static void SystemExact(UmiJsonWriter *writer,const char *key,uint64_t value,bool known)
{
    SystemKey(writer,key);
    if(!known){UmiJsonWriterRaw(writer,"null");return;}
    /* JSON consumers often use floating point. Decimal strings keep every
     * observed byte count and identity revision intact across that boundary. */
    char text[32];int length=snprintf(text,sizeof text,"%" PRIu64,value);
    if(length<0 || (size_t)length>=sizeof text){
        if(writer->status==UMI_STATUS_OK)writer->status=UMI_STATUS_INVALID_STATE;
        return;
    }
    UmiJsonWriterString(writer,text);
}
static void SystemDigest(UmiJsonWriter *writer,const char *key,const unsigned char *digest)
{
    char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(digest,hex);
    SystemText(writer,key,hex,true);
}
UmiStatus UmiComfySystemStatsJson(const UmiComfySystemStats *stats,char *out,
    size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(capacity==0U)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfySystemSummary summary;
    UmiStatus status=UmiComfySystemStatsInspect(stats,&summary);
    if(status!=UMI_STATUS_OK)return status;
    size_t bound=capacity<UMI_COMFY_SYSTEM_JSON_CAPACITY?capacity:UMI_COMFY_SYSTEM_JSON_CAPACITY;
    char *staged=malloc(bound);if(staged==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;UmiJsonWriterInit(&writer,staged,bound);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"system-observations\"");
    SystemExact(&writer,"connection_revision",summary.connection_revision,true);
    SystemDigest(&writer,"connection_sha256",summary.connection_sha256);
    SystemDigest(&writer,"response_sha256",summary.response_sha256);
    SystemCount(&writer,"response_bytes",summary.response_bytes);
    SystemCount(&writer,"device_count",summary.device_count);
    SystemCount(&writer,"known",summary.known);
    SystemText(&writer,"os",summary.os,(summary.known&UMI_COMFY_SYSTEM_OS)!=0U);
    SystemText(&writer,"python_version",summary.python_version,(summary.known&UMI_COMFY_SYSTEM_PYTHON)!=0U);
    SystemText(&writer,"pytorch_version",summary.pytorch_version,(summary.known&UMI_COMFY_SYSTEM_PYTORCH)!=0U);
    SystemText(&writer,"comfyui_version",summary.comfyui_version,(summary.known&UMI_COMFY_SYSTEM_VERSION)!=0U);
    SystemBoolean(&writer,"embedded_python",summary.embedded_python,(summary.known&UMI_COMFY_SYSTEM_EMBEDDED)!=0U);
    SystemExact(&writer,"ram_total",summary.ram_total,(summary.known&UMI_COMFY_SYSTEM_RAM_TOTAL)!=0U);
    SystemExact(&writer,"ram_free",summary.ram_free,(summary.known&UMI_COMFY_SYSTEM_RAM_FREE)!=0U);
    SystemBoolean(&writer,"ram_inconsistent",summary.ram_inconsistent,true);
    UmiJsonWriterRaw(&writer,",\"devices\":[");
    for(size_t i=0U;i<summary.device_count;++i){
        UmiComfyDeviceObservation device;
        status=UmiComfySystemStatsDeviceAt(stats,i,&device);
        if(status!=UMI_STATUS_OK)goto done;
        if(i!=0U)UmiJsonWriterRaw(&writer,",");
        UmiJsonWriterRaw(&writer,"{\"name\":");UmiJsonWriterString(&writer,device.name);
        SystemText(&writer,"type",device.type,true);
        SystemCount(&writer,"response_index",i);SystemCount(&writer,"known",device.known);
        SystemExact(&writer,"index",device.index,(device.known&UMI_COMFY_DEVICE_INDEX)!=0U);
        SystemExact(&writer,"vram_total",device.vram_total,(device.known&UMI_COMFY_DEVICE_VRAM_TOTAL)!=0U);
        SystemExact(&writer,"vram_free",device.vram_free,(device.known&UMI_COMFY_DEVICE_VRAM_FREE)!=0U);
        SystemExact(&writer,"torch_vram_total",device.torch_vram_total,(device.known&UMI_COMFY_DEVICE_TORCH_TOTAL)!=0U);
        SystemExact(&writer,"torch_vram_free",device.torch_vram_free,(device.known&UMI_COMFY_DEVICE_TORCH_FREE)!=0U);
        SystemBoolean(&writer,"vram_inconsistent",device.vram_inconsistent,true);
        SystemBoolean(&writer,"torch_vram_inconsistent",device.torch_vram_inconsistent,true);
        UmiJsonWriterRaw(&writer,"}");umi_secret_clear(&device,sizeof device);
    }
    UmiJsonWriterRaw(&writer,"]}");status=writer.status;
    /* All parsing and formatting precedes the only caller-memory mutation.
     * Insufficient capacity never leaves a partial report or length behind. */
    if(status==UMI_STATUS_OK){memcpy(out,staged,writer.length+1U);*out_length=writer.length;}
done:
    umi_secret_clear(staged,bound);free(staged);umi_secret_clear(&summary,sizeof summary);
    return status;
}
