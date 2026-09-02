/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/store.c
 *
 * PURPOSE:
 *   Implement the Data Server store adapter and guarded convenience calls over provider-neutral function tables.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/store.h"

#include <string.h>

/* Copy server into module-owned storage so callers keep ownership of their input values. */
static UmiStatus server_set(void *instance, const char *key, const char *value)
{
    return umi_data_server_set((UmiDataServer *)instance, key, value);
}

/* Provide the server get operation used by this module and its client applications. */
static UmiStatus server_get(void *instance,
                            const char *key,
                            char *value,
                            size_t value_capacity)
{
    return umi_data_server_get((const UmiDataServer *)instance,
                               key,
                               value,
                               value_capacity);
}

/* Remove server while keeping the remaining records in a valid and discoverable state. */
static UmiStatus server_remove(void *instance, const char *key)
{
    return umi_data_server_delete((UmiDataServer *)instance, key);
}

/* Return the number of records represented by server without changing their state. */
static size_t server_count(void *instance)
{
    return umi_data_server_count((const UmiDataServer *)instance);
}

/*
 * Provide the store from data server operation used by this module and its client
 * applications.
 */
UmiStatus umi_store_from_data_server(UmiDataServer *server,
                                     UmiStore *out_store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_store, 0, sizeof(*out_store));
    out_store->structure_size = (uint32_t)sizeof(*out_store);
    out_store->instance = server;
    out_store->set = server_set;
    out_store->get = server_get;
    out_store->remove = server_remove;
    out_store->count = server_count;
    return UMI_STATUS_OK;
}

/* Copy store into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_store_set(UmiStore *store,
                        const char *key,
                        const char *value)
{
    return store != NULL && store->set != NULL
        ? store->set(store->instance, key, value)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the store get operation used by this module and its client applications. */
UmiStatus umi_store_get(const UmiStore *store,
                        const char *key,
                        char *value,
                        size_t value_capacity)
{
    return store != NULL && store->get != NULL
        ? store->get(store->instance, key, value, value_capacity)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Remove store while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_store_remove(UmiStore *store, const char *key)
{
    return store != NULL && store->remove != NULL
        ? store->remove(store->instance, key)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Return the number of records represented by store without changing their state. */
size_t umi_store_count(const UmiStore *store)
{
    return store != NULL && store->count != NULL
        ? store->count(store->instance)
        : 0U;
}
