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

/*
 * Initialise ai developer durable store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_durable_store_create(
    const char *path,
    UmiAiDeveloperDurableStore **out_store)
{
    UmiAiDeveloperDurableStore *owner;
    size_t length;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(owner->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_store = NULL;

    owner = (UmiAiDeveloperDurableStore *)calloc(1U, sizeof(*owner));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_session_store_create(&owner->store);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(owner);
        return status;
    }

    (void)memcpy(owner->path, path, length + 1U);
    owner->revision = 1U;
    *out_store = owner;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai developer durable store so the same storage can be
 * reused safely.
 */
void umi_ai_developer_durable_store_destroy(
    UmiAiDeveloperDurableStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;

    umi_session_store_destroy(store->store);
    free(store);
}

/*
 * Read ai developer durable store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_ai_developer_durable_store_load(
    UmiAiDeveloperDurableStore *store)
{
    int loaded = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || store->store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_session_store_load(
        store->store,
        store->path,
        &loaded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    store->loaded = loaded;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Write ai developer durable store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_durable_store_save(
    UmiAiDeveloperDurableStore *store)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || store->store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_session_store_save(store->store, store->path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

/*
 * Provide the ai developer durable store session operation used by this module and its
 * client applications.
 */
UmiSessionStore *umi_ai_developer_durable_store_session(
    UmiAiDeveloperDurableStore *store)
{
    return store != NULL ? store->store : NULL;
}
