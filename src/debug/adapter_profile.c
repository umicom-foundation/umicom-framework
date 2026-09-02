/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/adapter_profile.c
 *
 * PURPOSE:
 *   Implement the adapter profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework debugger adapter profiles. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/adapter_profile.h"
#include <stdlib.h>
#include <string.h>
struct UmiDebugAdapterProfileRegistry{UmiDebugAdapterProfile items[UMI_DEBUG_ADAPTER_PROFILE_CAPACITY];size_t count;uint64_t revision;};
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDebugAdapterProfileRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise debug adapter profile registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_adapter_profile_registry_create(UmiDebugAdapterProfileRegistry**out){UmiDebugAdapterProfileRegistry*r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by debug adapter profile registry so the same storage can be
 * reused safely.
 */
void umi_debug_adapter_profile_registry_destroy(UmiDebugAdapterProfileRegistry*r){free(r);}
/*
 * Provide the debug adapter profile registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_adapter_profile_registry_upsert(UmiDebugAdapterProfileRegistry*r,const UmiDebugAdapterProfile*p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||p==NULL||p->id[0]=='\0'||p->executable[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,p->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_DEBUG_ADAPTER_PROFILE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*p;r->items[i].id[127U]='\0';r->items[i].display_name[255U]='\0';r->items[i].executable[1023U]='\0';r->items[i].arguments[2047U]='\0';r->items[i].debugger_kind[127U]='\0';r->revision+=1U;r->items[i].revision=r->revision;return UMI_STATUS_OK;}
/*
 * Find debug adapter profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_adapter_profile_registry_find(const UmiDebugAdapterProfileRegistry*r,const char*id,UmiDebugAdapterProfile*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Find debug adapter profile registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_adapter_profile_registry_at(const UmiDebugAdapterProfileRegistry*r,size_t i,UmiDebugAdapterProfile*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by debug adapter profile registry without
 * changing their state.
 */
size_t umi_debug_adapter_profile_registry_count(const UmiDebugAdapterProfileRegistry*r){return r!=NULL?r->count:0U;}
