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
/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiAccessibilityModel *model,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<model->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(model->items[i].node_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui accessibility model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_accessibility_model_create(UmiUiAccessibilityModel **out_model){UmiUiAccessibilityModel *m;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui accessibility model so the same storage can be reused
 * safely.
 */
void umi_ui_accessibility_model_destroy(UmiUiAccessibilityModel *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
/*
 * Provide the ui accessibility model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_accessibility_model_upsert(UmiUiAccessibilityModel *m,const UmiUiAccessibilitySnapshot *item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->node_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->node_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_UI_ACCESSIBILITY_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui accessibility model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_accessibility_model_remove(UmiUiAccessibilityModel *m,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui accessibility model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_accessibility_model_find(const UmiUiAccessibilityModel *m,const char *id,UmiUiAccessibilitySnapshot *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui accessibility model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_accessibility_model_at(const UmiUiAccessibilityModel *m,size_t index,UmiUiAccessibilitySnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Return the number of records represented by ui accessibility model without changing
 * their state.
 */
size_t umi_ui_accessibility_model_count(const UmiUiAccessibilityModel *m){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
/*
 * Provide the ui accessibility model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_accessibility_model_revision(const UmiUiAccessibilityModel *m){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
