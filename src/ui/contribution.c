/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/contribution.c
 *
 * PURPOSE:
 *   Implement define contributions from framework modules and plug-ins to panes, actions,
 *   menus, toolbars, status items and perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/contribution.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiContributionModel { UmiUiContributionSnapshot items[UMI_UI_CONTRIBUTION_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiContributionModel *model,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<model->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(model->items[i].contribution_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui contribution model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_contribution_model_create(UmiUiContributionModel **out_model){UmiUiContributionModel *m;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui contribution model so the same storage can be reused
 * safely.
 */
void umi_ui_contribution_model_destroy(UmiUiContributionModel *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
/*
 * Provide the ui contribution model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_contribution_model_upsert(UmiUiContributionModel *m,const UmiUiContributionSnapshot *item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->contribution_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->contribution_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_UI_CONTRIBUTION_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Remove ui contribution model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_contribution_model_remove(UmiUiContributionModel *m,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui contribution model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_contribution_model_find(const UmiUiContributionModel *m,const char *id,UmiUiContributionSnapshot *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Find ui contribution model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_contribution_model_at(const UmiUiContributionModel *m,size_t index,UmiUiContributionSnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
/*
 * Return the number of records represented by ui contribution model without changing their
 * state.
 */
size_t umi_ui_contribution_model_count(const UmiUiContributionModel *m){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
/*
 * Provide the ui contribution model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_contribution_model_revision(const UmiUiContributionModel *m){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
