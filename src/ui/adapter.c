/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adapter.c
 *
 * PURPOSE:
 *   Implement validated frontend-adapter registration and deterministic destruction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adapter.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiAdapterRegistry { UmiUiAdapter adapters[UMI_UI_ADAPTER_MAX]; size_t count; UmiMutex *mutex; };
static size_t find_adapter(const UmiUiAdapterRegistry *r,const char *id){size_t i;for(i=0U;i<r->count;++i)if(strcmp(r->adapters[i].adapter_id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_adapter_registry_create(UmiUiAdapterRegistry **out){UmiUiAdapterRegistry *r;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&r->mutex);if(s!=UMI_STATUS_OK){free(r);return s;}*out=r;return UMI_STATUS_OK;}
void umi_ui_adapter_registry_destroy(UmiUiAdapterRegistry *r){size_t i;if(r==NULL)return;for(i=r->count;i>0U;--i)if(r->adapters[i-1U].destroy!=NULL)r->adapters[i-1U].destroy(r->adapters[i-1U].instance);umi_mutex_destroy(r->mutex);free(r);}
UmiStatus umi_ui_adapter_registry_register(UmiUiAdapterRegistry *r,const UmiUiAdapter *a){if(r==NULL||a==NULL||a->structure_size<sizeof(*a)||a->abi_version!=UMI_UI_ADAPTER_ABI_VERSION||!umi_ui_id_is_valid(a->adapter_id)||a->present==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);if(find_adapter(r,a->adapter_id)!=SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_ALREADY_EXISTS;}if(r->count>=UMI_UI_ADAPTER_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}r->adapters[r->count++]=*a;(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_adapter_registry_unregister(UmiUiAdapterRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_adapter(r,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}if(r->adapters[i].destroy!=NULL)r->adapters[i].destroy(r->adapters[i].instance);if(i+1U<r->count)(void)memmove(&r->adapters[i],&r->adapters[i+1U],(r->count-i-1U)*sizeof(r->adapters[0]));r->count--; (void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
const UmiUiAdapter *umi_ui_adapter_registry_find(const UmiUiAdapterRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return NULL;i=find_adapter(r,id);return i!=SIZE_MAX?&r->adapters[i]:NULL;}
size_t umi_ui_adapter_registry_count(const UmiUiAdapterRegistry *r){return r!=NULL?r->count:0U;}
