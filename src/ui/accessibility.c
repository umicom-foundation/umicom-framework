/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/accessibility.c
 *
 * PURPOSE:
 *   Implement define accessible names, descriptions, roles, states and parent relationships
 *   that all frontend adapters must preserve.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/accessibility.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiAccessibilityModel { UmiUiAccessibilitySnapshot items[UMI_UI_ACCESSIBILITY_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
static size_t find_item(const UmiUiAccessibilityModel *model,const char *id){size_t i;for(i=0U;i<model->count;++i)if(strcmp(model->items[i].node_id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_accessibility_model_create(UmiUiAccessibilityModel **out_model){UmiUiAccessibilityModel *m;UmiStatus s;if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
void umi_ui_accessibility_model_destroy(UmiUiAccessibilityModel *m){if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
UmiStatus umi_ui_accessibility_model_upsert(UmiUiAccessibilityModel *m,const UmiUiAccessibilitySnapshot *item){size_t i;if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->node_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->node_id);if(i==SIZE_MAX){if(m->count>=UMI_UI_ACCESSIBILITY_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_accessibility_model_remove(UmiUiAccessibilityModel *m,const char *id){size_t i;if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_accessibility_model_find(const UmiUiAccessibilityModel *m,const char *id,UmiUiAccessibilitySnapshot *out){size_t i;if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_accessibility_model_at(const UmiUiAccessibilityModel *m,size_t index,UmiUiAccessibilitySnapshot *out){if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
size_t umi_ui_accessibility_model_count(const UmiUiAccessibilityModel *m){size_t n=0U;if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
uint64_t umi_ui_accessibility_model_revision(const UmiUiAccessibilityModel *m){uint64_t r=0U;if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
