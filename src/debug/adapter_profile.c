/* Umicom Framework debugger adapter profiles. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/adapter_profile.h"
#include <stdlib.h>
#include <string.h>
struct UmiDebugAdapterProfileRegistry{UmiDebugAdapterProfile items[UMI_DEBUG_ADAPTER_PROFILE_CAPACITY];size_t count;uint64_t revision;};
static size_t find_index(const UmiDebugAdapterProfileRegistry*r,const char*id){size_t i;if(r==NULL||id==NULL)return SIZE_MAX;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_debug_adapter_profile_registry_create(UmiDebugAdapterProfileRegistry**out){UmiDebugAdapterProfileRegistry*r;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
void umi_debug_adapter_profile_registry_destroy(UmiDebugAdapterProfileRegistry*r){free(r);}
UmiStatus umi_debug_adapter_profile_registry_upsert(UmiDebugAdapterProfileRegistry*r,const UmiDebugAdapterProfile*p){size_t i;if(r==NULL||p==NULL||p->id[0]=='\0'||p->executable[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,p->id);if(i==SIZE_MAX){if(r->count>=UMI_DEBUG_ADAPTER_PROFILE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*p;r->items[i].id[127U]='\0';r->items[i].display_name[255U]='\0';r->items[i].executable[1023U]='\0';r->items[i].arguments[2047U]='\0';r->items[i].debugger_kind[127U]='\0';r->revision+=1U;r->items[i].revision=r->revision;return UMI_STATUS_OK;}
UmiStatus umi_debug_adapter_profile_registry_find(const UmiDebugAdapterProfileRegistry*r,const char*id,UmiDebugAdapterProfile*out){size_t i;if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
UmiStatus umi_debug_adapter_profile_registry_at(const UmiDebugAdapterProfileRegistry*r,size_t i,UmiDebugAdapterProfile*out){if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
size_t umi_debug_adapter_profile_registry_count(const UmiDebugAdapterProfileRegistry*r){return r!=NULL?r->count:0U;}
