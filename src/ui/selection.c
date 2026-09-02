/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/selection.c
 *
 * PURPOSE:
 *   Implement a thread-safe, bounded and revisioned multi-selection model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/selection.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiSelectionModel { char ids[UMI_UI_SELECTION_MAX][UMI_UI_ID_CAPACITY]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Provide the find id operation used by this module and its client applications. */
static size_t find_id(const UmiUiSelectionModel *s,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->ids[i],id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui selection from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_selection_create(UmiUiSelectionModel **out){UmiUiSelectionModel *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;st=umi_mutex_create(&s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK){free(s);return st;}s->revision=1U;*out=s;return UMI_STATUS_OK;}
/* Release or reset state held by ui selection so the same storage can be reused safely. */
void umi_ui_selection_destroy(UmiUiSelectionModel *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;umi_mutex_destroy(s->mutex);free(s);}
/*
 * Provide the ui selection set single operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_selection_set_single(UmiUiSelectionModel *s,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_ui_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);s->count=1U;(void)umi_ui_copy_text(s->ids[0],sizeof(s->ids[0]),id);s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/* Add ui selection only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_selection_add(UmiUiSelectionModel *s,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_ui_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_id(s,id)!=SIZE_MAX){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_ALREADY_EXISTS;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_UI_SELECTION_MAX){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_ui_copy_text(s->ids[s->count++],UMI_UI_ID_CAPACITY,id);s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui selection while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_selection_remove(UmiUiSelectionModel *s,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);i=find_id(s,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<s->count)(void)memmove(&s->ids[i],&s->ids[i+1U],(s->count-i-1U)*sizeof(s->ids[0]));s->count--;s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/* Release or reset state held by ui selection so the same storage can be reused safely. */
void umi_ui_selection_clear(UmiUiSelectionModel *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;(void)umi_mutex_lock(s->mutex);s->count=0U;s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);}
/*
 * Provide the ui selection contains operation used by this module and its client
 * applications.
 */
int umi_ui_selection_contains(const UmiUiSelectionModel *s,const char *id){int found=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL&&id!=NULL){(void)umi_mutex_lock(s->mutex);found=find_id(s,id)!=SIZE_MAX;(void)umi_mutex_unlock(s->mutex);}return found;}
/* Return the number of records represented by ui selection without changing their state. */
size_t umi_ui_selection_count(const UmiUiSelectionModel *s){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){(void)umi_mutex_lock(s->mutex);n=s->count;(void)umi_mutex_unlock(s->mutex);}return n;}
/*
 * Provide the ui selection revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_selection_revision(const UmiUiSelectionModel *s){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){(void)umi_mutex_lock(s->mutex);r=s->revision;(void)umi_mutex_unlock(s->mutex);}return r;}
/* Find ui selection while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_ui_selection_at(const UmiUiSelectionModel *s,size_t index,char *out,size_t cap){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=s->count){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_copy_text(out,cap,s->ids[index])){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
