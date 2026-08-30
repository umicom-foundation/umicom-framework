/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/durable_store.c
 *
 * PURPOSE:
 *   Compose the existing Framework Session Store as durable AI developer state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/durable_store.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_ai_developer_durable_store_create(
    const char *path,
    UmiAiDeveloperDurableStore **out_store)
{
    UmiAiDeveloperDurableStore *owner;
    size_t length;
    UmiStatus status;

    if (path == NULL || path[0] == '\0' || out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(path);
    if (length >= sizeof(owner->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_store = NULL;

    owner = (UmiAiDeveloperDurableStore *)calloc(1U, sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_session_store_create(&owner->store);
    if (status != UMI_STATUS_OK) {
        free(owner);
        return status;
    }

    (void)memcpy(owner->path, path, length + 1U);
    owner->revision = 1U;
    *out_store = owner;
    return UMI_STATUS_OK;
}

void umi_ai_developer_durable_store_destroy(
    UmiAiDeveloperDurableStore *store)
{
    if (store == NULL) return;

    umi_session_store_destroy(store->store);
    free(store);
}

UmiStatus umi_ai_developer_durable_store_load(
    UmiAiDeveloperDurableStore *store)
{
    int loaded = 0;
    UmiStatus status;

    if (store == NULL || store->store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_session_store_load(
        store->store,
        store->path,
        &loaded);
    if (status != UMI_STATUS_OK) return status;

    store->loaded = loaded;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_durable_store_save(
    UmiAiDeveloperDurableStore *store)
{
    UmiStatus status;

    if (store == NULL || store->store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_session_store_save(store->store, store->path);
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

UmiSessionStore *umi_ai_developer_durable_store_session(
    UmiAiDeveloperDurableStore *store)
{
    return store != NULL ? store->store : NULL;
}
