/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_system.c
 *
 * PURPOSE:
 *   Compose shared ComfyUI admission with bounded metadata ownership and explicit unknown counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_system.h"
#include "comfy_response_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfySystemStats {
    UmiComfySystemSummary summary;
    UmiComfyDeviceObservation devices[UMI_COMFY_SYSTEM_DEVICE_LIMIT];
};
/* System reports legitimately use null for CPU indices and unavailable fields.
 * Keep this optional-field policy local to the observation, rather than changing
 * required fields in existing submission, history or queue decoders. */
static UmiStatus OptionalNode(const UmiJsonTree *tree,int object,const char *name,int *out)
{
    UmiStatus status=UmiJsonTreeMember(tree,object,name,out);
    if(status==UMI_STATUS_NOT_FOUND){*out=-1;return UMI_STATUS_OK;}
    if(status==UMI_STATUS_OK && UmiJsonTreeIsNull(tree,*out))*out=-1;
    return status;
}
static UmiStatus OptionalText(const UmiJsonTree *tree,int object,const char *name,
    char *out,size_t capacity,uint32_t bit,uint32_t *known)
{
    int node=-1;UmiStatus status=OptionalNode(tree,object,name,&node);
    if(status!=UMI_STATUS_OK || node<0)return status;
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
    status=UmiJsonTreeText(tree,node,out,capacity);
    if(status==UMI_STATUS_OK)*known|=bit;
    return status;
}
static UmiStatus OptionalCount(const UmiJsonTree *tree,int object,const char *name,
    uint64_t *out,uint32_t bit,uint32_t *known)
{
    int node=-1;UmiStatus status=OptionalNode(tree,object,name,&node);
    if(status!=UMI_STATUS_OK || node<0)return status;
    int64_t value=0;status=UmiJsonTreeInteger(tree,node,&value);
    if(status!=UMI_STATUS_OK)return status;
    if(value<0)return UMI_STATUS_PARSE_ERROR;
    *out=(uint64_t)value;*known|=bit;return UMI_STATUS_OK;
}
static bool Inconsistent(uint32_t known,uint32_t total_bit,uint32_t free_bit,
    uint64_t total,uint64_t free_bytes)
{
    return (known&(total_bit|free_bit))==(total_bit|free_bit) && free_bytes>total;
}
static UmiStatus SystemRead(const UmiJsonTree *tree,int node,UmiComfySystemSummary *out)
{
    UmiStatus status=OptionalText(tree,node,"os",out->os,sizeof out->os,UMI_COMFY_SYSTEM_OS,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalText(tree,node,"python_version",out->python_version,sizeof out->python_version,UMI_COMFY_SYSTEM_PYTHON,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalText(tree,node,"pytorch_version",out->pytorch_version,sizeof out->pytorch_version,UMI_COMFY_SYSTEM_PYTORCH,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalText(tree,node,"comfyui_version",out->comfyui_version,sizeof out->comfyui_version,UMI_COMFY_SYSTEM_VERSION,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"ram_total",&out->ram_total,UMI_COMFY_SYSTEM_RAM_TOTAL,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"ram_free",&out->ram_free,UMI_COMFY_SYSTEM_RAM_FREE,&out->known);
    int embedded=-1;
    if(status==UMI_STATUS_OK)status=OptionalNode(tree,node,"embedded_python",&embedded);
    if(status==UMI_STATUS_OK && embedded>=0){
        int value=0;status=UmiJsonTreeBoolean(tree,embedded,&value);
        if(status==UMI_STATUS_OK){out->embedded_python=value!=0;out->known|=UMI_COMFY_SYSTEM_EMBEDDED;}
    }
    out->ram_inconsistent=Inconsistent(out->known,UMI_COMFY_SYSTEM_RAM_TOTAL,UMI_COMFY_SYSTEM_RAM_FREE,out->ram_total,out->ram_free);
    return status;
}
static UmiStatus DeviceRead(const UmiJsonTree *tree,int node,UmiComfyDeviceObservation *out)
{
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    UmiStatus status=UmiComfyResponseText(tree,node,"name",out->name,sizeof out->name,0);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseText(tree,node,"type",out->type,sizeof out->type,0);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"index",&out->index,UMI_COMFY_DEVICE_INDEX,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"vram_total",&out->vram_total,UMI_COMFY_DEVICE_VRAM_TOTAL,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"vram_free",&out->vram_free,UMI_COMFY_DEVICE_VRAM_FREE,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"torch_vram_total",&out->torch_vram_total,UMI_COMFY_DEVICE_TORCH_TOTAL,&out->known);
    if(status==UMI_STATUS_OK)status=OptionalCount(tree,node,"torch_vram_free",&out->torch_vram_free,UMI_COMFY_DEVICE_TORCH_FREE,&out->known);
    out->vram_inconsistent=Inconsistent(out->known,UMI_COMFY_DEVICE_VRAM_TOTAL,UMI_COMFY_DEVICE_VRAM_FREE,out->vram_total,out->vram_free);
    out->torch_vram_inconsistent=Inconsistent(out->known,UMI_COMFY_DEVICE_TORCH_TOTAL,UMI_COMFY_DEVICE_TORCH_FREE,out->torch_vram_total,out->torch_vram_free);
    return status;
}
void UmiComfySystemStatsDestroy(UmiComfySystemStats *stats)
{
    if(stats!=NULL){umi_secret_clear(stats,sizeof *stats);free(stats);}
}
UmiStatus UmiComfySystemStatsCreate(const UmiProviderConnection *connection,
    uint64_t revision,const void *bytes,size_t length,
    const UmiCancellationToken *cancel,UmiComfySystemStats **out)
{
    if(out==NULL || *out!=NULL || revision==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    UmiStatus status=UmiComfyConnectionCheck(connection);
    if(status!=UMI_STATUS_OK)return status;
    UmiJsonTree *tree=NULL;
    status=UmiComfyResponseOpen(bytes,length,UMI_COMFY_SYSTEM_MAX_BYTES,cancel,&tree);
    if(status!=UMI_STATUS_OK)return status;
    int system=-1,devices=-1;
    status=UmiComfyResponseMember(tree,0,"system",&system);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseMember(tree,0,"devices",&devices);
    if(status==UMI_STATUS_OK && (UmiJsonTreeKind(tree,system)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeKind(tree,devices)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY))status=UMI_STATUS_PARSE_ERROR;
    size_t count=0U;
    if(status==UMI_STATUS_OK){count=UmiJsonTreeCount(tree,devices);if(count>UMI_COMFY_SYSTEM_DEVICE_LIMIT)status=UMI_STATUS_CAPACITY_EXCEEDED;}
    UmiComfySystemStats *candidate=NULL;
    if(status==UMI_STATUS_OK){candidate=calloc(1U,sizeof *candidate);if(candidate==NULL)status=UMI_STATUS_OUT_OF_MEMORY;}
    if(status==UMI_STATUS_OK){
        candidate->summary.device_count=count;candidate->summary.response_bytes=length;
        candidate->summary.connection_revision=revision;
        status=SystemRead(tree,system,&candidate->summary);
    }
    size_t index=0U;
    if(status==UMI_STATUS_OK)for(int node=UmiJsonTreeFirst(tree,devices);node>=0;node=UmiJsonTreeNext(tree,node)){
        if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
        status=DeviceRead(tree,node,&candidate->devices[index++]);
        if(status!=UMI_STATUS_OK)break;
    }
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionFingerprint(connection,candidate->summary.connection_sha256);
    if(status==UMI_STATUS_OK){
        UmiSha256 digest;UmiSha256Init(&digest);status=UmiSha256Update(&digest,bytes,length);
        if(status==UMI_STATUS_OK)status=UmiSha256Final(&digest,candidate->summary.response_sha256);
        umi_secret_clear(&digest,sizeof digest);
    }
    /* Only copied allowlisted fields survive the temporary JSON owner. */
    UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)*out=candidate;else UmiComfySystemStatsDestroy(candidate);
    return status;
}
UmiStatus UmiComfySystemStatsInspect(const UmiComfySystemStats *stats,UmiComfySystemSummary *out)
{
    if(stats==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=stats->summary;return UMI_STATUS_OK;
}
UmiStatus UmiComfySystemStatsDeviceAt(const UmiComfySystemStats *stats,size_t index,UmiComfyDeviceObservation *out)
{
    if(stats==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=stats->summary.device_count)return UMI_STATUS_NOT_FOUND;
    *out=stats->devices[index];return UMI_STATUS_OK;
}
UmiStatus UmiComfySystemStatsCheckConnection(const UmiComfySystemStats *stats,
    const UmiProviderConnection *current,uint64_t revision)
{
    if(stats==NULL || current==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiComfyConnectionCheck(current);if(status!=UMI_STATUS_OK)return status;
    if(revision!=stats->summary.connection_revision)return UMI_STATUS_BUSY;
    unsigned char digest[UMI_SHA256_BYTES];status=UmiProviderConnectionFingerprint(current,digest);
    if(status==UMI_STATUS_OK && memcmp(digest,stats->summary.connection_sha256,sizeof digest)!=0)status=UMI_STATUS_BUSY;
    return status;
}
