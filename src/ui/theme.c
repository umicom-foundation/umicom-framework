/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/theme.c
 *
 * PURPOSE:
 *   Implement define semantic theme tokens so product code does not depend on toolkit-specific
 *   css selectors, colours or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/theme.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"
#include "theme_validation_internal.h"

struct UmiUiThemeModel { UmiUiThemeSnapshot items[UMI_UI_THEME_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiThemeModel *model,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<model->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(model->items[i].token_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui theme model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_theme_model_create(UmiUiThemeModel **out_model){UmiUiThemeModel *m;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_model==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_model=NULL;m=calloc(1U,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(m);return s;}m->revision=1U;*out_model=m;return UMI_STATUS_OK;}
/* Release or reset state held by ui theme model so the same storage can be reused safely. */
void umi_ui_theme_model_destroy(UmiUiThemeModel *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return;umi_mutex_destroy(m->mutex);free(m);}
/*
 * Provide the ui theme model upsert operation used by this module and its client
 * applications.
 */
/* Complete bounded text admission replaces ID-only admission so native adapters never receive an unterminated stored field. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_theme_model_upsert(UmiUiThemeModel *m,const UmiUiThemeSnapshot *item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||item==NULL||!umi_ui_id_is_valid(item->token_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,item->token_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_UI_THEME_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=m->count++;}m->items[i]=*item;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_theme_model_upsert(UmiUiThemeModel *m, const UmiUiThemeSnapshot *item)
{
    if (m == NULL || item == NULL ||
        UmiStorageOverlaps(m, sizeof(*m), item, sizeof(*item)))
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The token store owns complete value copies. Validate the same snapshot
     * that will be committed, before capacity or revision can change. Values
     * remain semantic text; their renderer-specific meaning is checked later. */
    UmiUiThemeSnapshot staged = *item;
    if (!umi_ui_id_is_valid(staged.token_id) ||
        !UmiUiThemeTextIsBounded(staged.value, sizeof(staged.value), false) ||
        !UmiUiThemeTextIsBounded(staged.description, sizeof(staged.description), false))
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiStatus status = umi_mutex_lock(m->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = find_item(m, staged.token_id);
    if (index == SIZE_MAX) {
        if (m->count >= UMI_UI_THEME_MAX) {
            (void)umi_mutex_unlock(m->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = m->count++;
    }
    m->items[index] = staged;
    m->revision = umi_ui_next_revision(m->revision);
    (void)umi_mutex_unlock(m->mutex);
    return UMI_STATUS_OK;
}
/*
 * Remove ui theme model while keeping the remaining records in a valid and discoverable
 * state.
 */
/* Bounded shared identifier admission replaces the unbounded external query; successful removal retains ordered compaction and clears the retired slot. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_theme_model_remove(UmiUiThemeModel *m,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<m->count)(void)memmove(&m->items[i],&m->items[i+1U],(m->count-i-1U)*sizeof(m->items[0]));m->count--;m->revision=umi_ui_next_revision(m->revision);(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_theme_model_remove(UmiUiThemeModel *m, const char *id)
{
    if (m == NULL || !umi_ui_id_is_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_mutex_lock(m->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = find_item(m, id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(m->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < m->count)
        (void)memmove(&m->items[index], &m->items[index + 1U],
                      (m->count - index - 1U) * sizeof(m->items[0]));
    --m->count;
    (void)memset(&m->items[m->count], 0, sizeof(m->items[0]));
    m->revision = umi_ui_next_revision(m->revision);
    (void)umi_mutex_unlock(m->mutex);
    return UMI_STATUS_OK;
}
/*
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Bounded lookup and independent output storage replace unchecked query comparison and protect the opaque model owner. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_theme_model_find(const UmiUiThemeModel *m,const char *id,UmiUiThemeSnapshot *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);i=find_item(m,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[i];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_theme_model_find(const UmiUiThemeModel *m, const char *id,
                                  UmiUiThemeSnapshot *out)
{
    if (m == NULL || out == NULL ||
        UmiStorageOverlaps(m, sizeof(*m), out, sizeof(*out)) ||
        !umi_ui_id_is_valid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_mutex_lock(m->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = find_item(m, id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(m->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Query comparison finishes before output publication. This permits a
     * query borrowed from the separate output snapshot without erasing it. */
    *out = m->items[index];
    (void)umi_mutex_unlock(m->mutex);
    return UMI_STATUS_OK;
}
/*
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Checked output ownership and lock acquisition preserve the value-copy API without allowing publication into the model allocation. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_theme_model_at(const UmiUiThemeModel *m,size_t index,UmiUiThemeSnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(m->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=m->count){(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_NOT_FOUND;}*out=m->items[index];(void)umi_mutex_unlock(m->mutex);return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_theme_model_at(const UmiUiThemeModel *m, size_t index,
                                UmiUiThemeSnapshot *out)
{
    if (m == NULL || out == NULL ||
        UmiStorageOverlaps(m, sizeof(*m), out, sizeof(*out)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_mutex_lock(m->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (index >= m->count) {
        (void)umi_mutex_unlock(m->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out = m->items[index];
    (void)umi_mutex_unlock(m->mutex);
    return UMI_STATUS_OK;
}
/* Return the number of records represented by ui theme model without changing their state. */
size_t umi_ui_theme_model_count(const UmiUiThemeModel *m){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);n=m->count;(void)umi_mutex_unlock(m->mutex);}return n;}
/*
 * Provide the ui theme model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_theme_model_revision(const UmiUiThemeModel *m){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){(void)umi_mutex_lock(m->mutex);r=m->revision;(void)umi_mutex_unlock(m->mutex);}return r;}
