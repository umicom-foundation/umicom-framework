/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/binding.c
 *
 * PURPOSE:
 *   Implement one-way view-model property binding and optional value transformation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/binding.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiBindingRegistry { UmiUiBindingDescriptor items[UMI_UI_BINDING_MAX]; size_t count; UmiMutex *mutex; };
static size_t find_binding(const UmiUiBindingRegistry *r,const char *id){size_t i;for(i=0U;i<r->count;++i)if(strcmp(r->items[i].binding_id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_binding_registry_create(UmiUiBindingRegistry **out){UmiUiBindingRegistry *r;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&r->mutex);if(s!=UMI_STATUS_OK){free(r);return s;}*out=r;return UMI_STATUS_OK;}
void umi_ui_binding_registry_destroy(UmiUiBindingRegistry *r){if(r==NULL)return;umi_mutex_destroy(r->mutex);free(r);}
UmiStatus umi_ui_binding_register(UmiUiBindingRegistry *r,const UmiUiBindingDescriptor *d){if(r==NULL||d==NULL||!umi_ui_id_is_valid(d->binding_id)||d->source_view==NULL||d->target_view==NULL||d->source_property[0]=='\0'||d->target_property[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);if(find_binding(r,d->binding_id)!=SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_ALREADY_EXISTS;}if(r->count>=UMI_UI_BINDING_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}r->items[r->count++]=*d;(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_binding_unregister(UmiUiBindingRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_binding(r,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}if(i+1U<r->count)(void)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--; (void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
static UmiStatus propagate(UmiUiBindingDescriptor *d){UmiUiValue source_value,target_value;UmiStatus s;if(!d->enabled)return UMI_STATUS_OK;s=umi_ui_view_model_get_property(d->source_view,d->source_property,&source_value);if(s!=UMI_STATUS_OK)return s;if(d->transform!=NULL)s=d->transform(&source_value,&target_value,d->user_data);else target_value=source_value;if(s!=UMI_STATUS_OK)return s;return umi_ui_view_model_set_property(d->target_view,d->target_property,&target_value);}
UmiStatus umi_ui_binding_propagate(UmiUiBindingRegistry *r,const char *id){size_t i;UmiStatus s;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_binding(r,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}s=propagate(&r->items[i]);(void)umi_mutex_unlock(r->mutex);return s;}
UmiStatus umi_ui_binding_propagate_all(UmiUiBindingRegistry *r,size_t *out_count){size_t i,n=0U;UmiStatus first=UMI_STATUS_OK;if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);for(i=0U;i<r->count;++i){UmiStatus s=propagate(&r->items[i]);if(s==UMI_STATUS_OK)n++;else if(first==UMI_STATUS_OK)first=s;}(void)umi_mutex_unlock(r->mutex);if(out_count!=NULL)*out_count=n;return first;}
size_t umi_ui_binding_count(const UmiUiBindingRegistry *r){size_t n=0U;if(r!=NULL){(void)umi_mutex_lock(r->mutex);n=r->count;(void)umi_mutex_unlock(r->mutex);}return n;}
