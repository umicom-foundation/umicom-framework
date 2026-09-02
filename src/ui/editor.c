/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/editor.c
 *
 * PURPOSE:
 *   Implement define editor presentation state that remains independent from gtksourceview and
 *   other concrete text-editor widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/editor.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiEditorModel { UmiUiEditorSnapshot items[UMI_UI_EDITOR_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiEditorModel *model,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<model->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(model->items[i].editor_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui editor model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_editor_model_create(UmiUiEditorModel **out_model){UmiUiEditorModel *m;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
/* Release or reset state held by ui editor model so the same storage can be reused safely. */
void umi_ui_editor_model_destroy(UmiUiEditorModel *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
/*
 * Provide the ui editor model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_editor_model_upsert(UmiUiEditorModel *m,const UmiUiEditorSnapshot *item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->editor_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->editor_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_UI_EDITOR_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui editor model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_editor_model_remove(UmiUiEditorModel *m,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui editor model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_editor_model_find(const UmiUiEditorModel *m,const char *id,UmiUiEditorSnapshot *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui editor model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_editor_model_at(const UmiUiEditorModel *m,size_t index,UmiUiEditorSnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Return the number of records represented by ui editor model without changing their
 * state.
 */
size_t umi_ui_editor_model_count(const UmiUiEditorModel *m){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
/*
 * Provide the ui editor model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_editor_model_revision(const UmiUiEditorModel *m){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
