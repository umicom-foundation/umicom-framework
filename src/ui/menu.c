/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/menu.c
 *
 * PURPOSE:
 *   Implement define a toolkit-neutral menu contribution model whose items reference registered
 *   actions instead of toolkit callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/menu.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiMenuModel { UmiUiMenuSnapshot items[UMI_UI_MENU_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
static size_t find_item(const UmiUiMenuModel *model,const char *id){size_t i;for(i=0U;i<model->count;++i)if(strcmp(model->items[i].item_id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_menu_model_create(UmiUiMenuModel **out_model){UmiUiMenuModel *m;UmiStatus s;if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
void umi_ui_menu_model_destroy(UmiUiMenuModel *m){if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
UmiStatus umi_ui_menu_model_upsert(UmiUiMenuModel *m,const UmiUiMenuSnapshot *item){size_t i;if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->item_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->item_id);if(i==SIZE_MAX){if(m->count>=UMI_UI_MENU_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_menu_model_remove(UmiUiMenuModel *m,const char *id){size_t i;if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_menu_model_find(const UmiUiMenuModel *m,const char *id,UmiUiMenuSnapshot *out){size_t i;if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_menu_model_at(const UmiUiMenuModel *m,size_t index,UmiUiMenuSnapshot *out){if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
size_t umi_ui_menu_model_count(const UmiUiMenuModel *m){size_t n=0U;if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
uint64_t umi_ui_menu_model_revision(const UmiUiMenuModel *m){uint64_t r=0U;if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
