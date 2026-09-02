/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adapter.c
 *
 * PURPOSE:
 *   Implement validated frontend-adapter registration and deterministic destruction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adapter.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiAdapterRegistry { UmiUiAdapter adapters[UMI_UI_ADAPTER_MAX]; size_t count; UmiMutex *mutex; };
/* Provide the find adapter operation used by this module and its client applications. */
static size_t find_adapter(const UmiUiAdapterRegistry *r,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(r->adapters[i].adapter_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui adapter registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_adapter_registry_create(UmiUiAdapterRegistry **out){UmiUiAdapterRegistry *r;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&r->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(r);return s;}*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui adapter registry so the same storage can be reused
 * safely.
 */
void umi_ui_adapter_registry_destroy(UmiUiAdapterRegistry *r){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return;/* Visit each bounded item once so every record receives the same rule. */ for(i=r->count;i>0U;--i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->adapters[i-1U].destroy!=NULL)r->adapters[i-1U].destroy(r->adapters[i-1U].instance);umi_mutex_destroy(r->mutex);free(r);}
/* Add ui adapter registry only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_adapter_registry_register(UmiUiAdapterRegistry *r,const UmiUiAdapter *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||a==NULL||a->structure_size<sizeof(*a)||a->abi_version!=UMI_UI_ADAPTER_ABI_VERSION||!umi_ui_id_is_valid(a->adapter_id)||a->present==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_adapter(r,a->adapter_id)!=SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_ALREADY_EXISTS;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_UI_ADAPTER_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}r->adapters[r->count++]=*a;(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui adapter registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_adapter_registry_unregister(UmiUiAdapterRegistry *r,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_adapter(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->adapters[i].destroy!=NULL)r->adapters[i].destroy(r->adapters[i].instance);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)(void)memmove(&r->adapters[i],&r->adapters[i+1U],(r->count-i-1U)*sizeof(r->adapters[0]));r->count--; (void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
/*
 * Find ui adapter registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiAdapter *umi_ui_adapter_registry_find(const UmiUiAdapterRegistry *r,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return NULL;i=find_adapter(r,id);return i!=SIZE_MAX?&r->adapters[i]:NULL;}
/*
 * Return the number of records represented by ui adapter registry without changing their
 * state.
 */
size_t umi_ui_adapter_registry_count(const UmiUiAdapterRegistry *r){return r!=NULL?r->count:0U;}
