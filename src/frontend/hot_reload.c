/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/hot_reload.c
 *
 * PURPOSE:
 *   Implement the hot reload behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/hot_reload.h"
#include <string.h>
/*
 * Initialise frontend hot reload from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_hot_reload_init(UmiFrontendHotReload *r,int enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));r->enabled=enabled?1:0;r->generation=1U;return UMI_STATUS_OK;}
/* The original synchronous queue/completion implementation is retained for
 * review. The active implementation below validates bounded state and guards
 * generation overflow; the additive snapshot API preserves edits made while
 * a preview build is running. Existing complete calls still consume all work. */
#if 0
/*
 * Provide the frontend hot reload queue operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_queue(UmiFrontendHotReload *r,const char *path,uint64_t revision){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!r->enabled)return UMI_STATUS_INVALID_STATE;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->pending_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->pending[i].path,path)==0){r->pending[i].revision=revision;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->pending_count>=UMI_FRONTEND_DEV_MAX_CHANGES)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->pending_count++;s=umi_frontend_dev_copy_text(r->pending[i].path,sizeof(r->pending[i].path),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){r->pending_count-=1U;return s;}r->pending[i].revision=revision;return UMI_STATUS_OK;}
/*
 * Provide the frontend hot reload complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_complete(UmiFrontendHotReload *r,uint64_t *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!r->enabled)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->pending_count==0U)return UMI_STATUS_NOT_FOUND;r->generation+=1U;r->completed_generation=r->generation;r->pending_count=0U;*out=r->completed_generation;return UMI_STATUS_OK;}

#endif

/* A preview can finish after another edit. Validate bounded caller-owned state
 * first, then commit a change without any fallible operation after mutation.
 * This service records work only; it does not watch files, build code or reload
 * toolkit objects. The host owns those operations and serialises these calls. */
static int ChangedFilesValid(const UmiFrontendChangedFile *files, size_t count)
{
    if (count > UMI_FRONTEND_DEV_MAX_CHANGES) return 0;
    for (size_t i = 0U; i < count; ++i) {
        if (files[i].path[0] == '\0' ||
            memchr(files[i].path, '\0', sizeof(files[i].path)) == NULL) return 0;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(files[i].path, files[j].path) == 0) return 0;
    }
    return 1;
}

static UmiStatus ReloadStateValid(const UmiFrontendHotReload *reload)
{
    if (reload == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!reload->enabled || reload->generation == 0U ||
        !ChangedFilesValid(reload->pending, reload->pending_count))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus umi_frontend_hot_reload_queue(UmiFrontendHotReload *reload,
                                       const char *path, uint64_t revision)
{
    UmiStatus status;
    char copied[UMI_FRONTEND_DEV_PATH_CAPACITY] = {0};
    size_t length = 0U;
    if (reload == NULL || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ReloadStateValid(reload);
    if (status != UMI_STATUS_OK) return status;
    /* Copy before mutation, including when the input names an existing row. */
    while (length < sizeof(copied) && path[length] != '\0') ++length;
    if (length == sizeof(copied)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(copied, path, length + 1U);
    for (size_t i = 0U; i < reload->pending_count; ++i) {
        if (strcmp(reload->pending[i].path, copied) == 0) {
            reload->pending[i].revision = revision;
            return UMI_STATUS_OK;
        }
    }
    if (reload->pending_count == UMI_FRONTEND_DEV_MAX_CHANGES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(reload->pending[reload->pending_count].path, copied, sizeof(copied));
    reload->pending[reload->pending_count].revision = revision;
    ++reload->pending_count;
    return UMI_STATUS_OK;
}

/* Retain the existing all-pending completion contract for synchronous callers.
 * Asynchronous consumers use capture/acknowledge to preserve newer revisions. */
UmiStatus umi_frontend_hot_reload_complete(UmiFrontendHotReload *reload,
                                          uint64_t *out_generation)
{
    UmiStatus status;
    if (out_generation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReloadStateValid(reload);
    if (status != UMI_STATUS_OK) return status;
    if (reload->pending_count == 0U) return UMI_STATUS_NOT_FOUND;
    if (reload->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++reload->generation;
    reload->completed_generation = reload->generation;
    reload->pending_count = 0U;
    *out_generation = reload->completed_generation;
    return UMI_STATUS_OK;
}

UmiStatus umi_frontend_hot_reload_capture(const UmiFrontendHotReload *reload,
                                         UmiFrontendReloadBatch *batch)
{
    UmiStatus status;
    if (batch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReloadStateValid(reload);
    if (status != UMI_STATUS_OK) return status;
    if (reload->pending_count == 0U) return UMI_STATUS_NOT_FOUND;
    memset(batch, 0, sizeof(*batch));
    batch->owner = reload;
    batch->base_generation = reload->generation;
    batch->count = reload->pending_count;
    memcpy(batch->changes, reload->pending,
           reload->pending_count * sizeof(reload->pending[0]));
    return UMI_STATUS_OK;
}

UmiStatus umi_frontend_hot_reload_acknowledge(UmiFrontendHotReload *reload,
                                             const UmiFrontendReloadBatch *batch,
                                             UmiFrontendReloadReceipt *receipt)
{
    UmiStatus status;
    size_t kept = 0U;
    size_t acknowledged = 0U;
    if (batch == NULL || receipt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReloadStateValid(reload);
    if (status != UMI_STATUS_OK) return status;
    if (batch->owner != reload || batch->base_generation != reload->generation ||
        batch->count == 0U || !ChangedFilesValid(batch->changes, batch->count))
        return UMI_STATUS_INVALID_STATE;
    if (reload->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < reload->pending_count; ++i) {
        int matches = 0;
        for (size_t j = 0U; j < batch->count; ++j) {
            if (reload->pending[i].revision == batch->changes[j].revision &&
                strcmp(reload->pending[i].path, batch->changes[j].path) == 0) {
                matches = 1;
                break;
            }
        }
        if (matches) ++acknowledged;
        else {
            if (kept != i) reload->pending[kept] = reload->pending[i];
            ++kept;
        }
    }
    reload->pending_count = kept;
    ++reload->generation;
    reload->completed_generation = reload->generation;
    receipt->generation = reload->generation;
    receipt->acknowledged = acknowledged;
    receipt->remaining = kept;
    return UMI_STATUS_OK;
}
