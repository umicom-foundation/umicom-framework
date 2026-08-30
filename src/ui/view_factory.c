/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/view_factory.c
 *
 * PURPOSE:
 *   Implement view-factory registration and toolkit-neutral view-model creation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/view_factory.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiViewFactoryRegistry { UmiUiViewFactoryDescriptor items[UMI_UI_VIEW_FACTORY_MAX]; size_t count; UmiMutex *mutex; };
static size_t find_factory(const UmiUiViewFactoryRegistry *r,const char *type){size_t i;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].view_type,type)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_view_factory_registry_create(UmiUiViewFactoryRegistry **out){UmiUiViewFactoryRegistry *r;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&r->mutex);if(s!=UMI_STATUS_OK){free(r);return s;}*out=r;return UMI_STATUS_OK;}
void umi_ui_view_factory_registry_destroy(UmiUiViewFactoryRegistry *r){if(r==NULL)return;umi_mutex_destroy(r->mutex);free(r);}
UmiStatus umi_ui_view_factory_register(UmiUiViewFactoryRegistry *r,const UmiUiViewFactoryDescriptor *d){if(r==NULL||d==NULL||!umi_ui_id_is_valid(d->view_type)||!umi_ui_id_is_valid(d->provider_id)||d->create==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);if(find_factory(r,d->view_type)!=SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_ALREADY_EXISTS;}if(r->count>=UMI_UI_VIEW_FACTORY_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}r->items[r->count++]=*d;(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_view_factory_unregister(UmiUiViewFactoryRegistry *r,const char *type){size_t i;if(r==NULL||type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_factory(r,type);if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}if(i+1U<r->count)(void)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--; (void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_view_factory_create_view(const UmiUiViewFactoryRegistry *r,const char *type,const char *id,UmiUiViewModel **out){size_t i;UmiUiViewCreateFn fn;void *data;if(r==NULL||type==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_factory(r,type);if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}fn=r->items[i].create;data=r->items[i].user_data;(void)umi_mutex_unlock(r->mutex);return fn(id,data,out);}
size_t umi_ui_view_factory_count(const UmiUiViewFactoryRegistry *r){return r!=NULL?r->count:0U;}
