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
/* Provide the find factory operation used by this module and its client applications. */
static size_t find_factory(const UmiUiViewFactoryRegistry *r,const char *type){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(r->items[i].view_type,type)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui view factory registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_view_factory_registry_create(UmiUiViewFactoryRegistry **out){UmiUiViewFactoryRegistry *r;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&r->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(r);return s;}*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui view factory registry so the same storage can be
 * reused safely.
 */
void umi_ui_view_factory_registry_destroy(UmiUiViewFactoryRegistry *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return;umi_mutex_destroy(r->mutex);free(r);}
/* Add ui view factory only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_view_factory_register(UmiUiViewFactoryRegistry *r,const UmiUiViewFactoryDescriptor *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||d==NULL||!umi_ui_id_is_valid(d->view_type)||!umi_ui_id_is_valid(d->provider_id)||d->create==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_factory(r,d->view_type)!=SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_ALREADY_EXISTS;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_UI_VIEW_FACTORY_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}r->items[r->count++]=*d;(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui view factory while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_view_factory_unregister(UmiUiViewFactoryRegistry *r,const char *type){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_factory(r,type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)(void)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--; (void)umi_mutex_unlock(r->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view factory create view operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_factory_create_view(const UmiUiViewFactoryRegistry *r,const char *type,const char *id,UmiUiViewModel **out){size_t i;UmiUiViewCreateFn fn;void *data;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||type==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(r->mutex);i=find_factory(r,type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(r->mutex);return UMI_STATUS_NOT_FOUND;}fn=r->items[i].create;data=r->items[i].user_data;(void)umi_mutex_unlock(r->mutex);return fn(id,data,out);}
/*
 * Return the number of records represented by ui view factory without changing their
 * state.
 */
size_t umi_ui_view_factory_count(const UmiUiViewFactoryRegistry *r){return r!=NULL?r->count:0U;}
