/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/store.h
 *
 * PURPOSE:
 *   Define a provider-neutral key/value store contract and a Data Server adapter used by repositories, caches and durable messaging.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_STORE_H
#define UMICOM_DATA_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the store data shared with callers of this public contract.
 */
typedef struct UmiStore {
    uint32_t structure_size;
    void *instance;
    UmiStatus (*set)(void *instance, const char *key, const char *value);
    UmiStatus (*get)(void *instance,
                     const char *key,
                     char *value,
                     size_t value_capacity);
    UmiStatus (*remove)(void *instance, const char *key);
    size_t (*count)(void *instance);
} UmiStore;

/**
 * Provide the store from data server operation used by this module and its client
 * applications.
 */
UmiStatus umi_store_from_data_server(UmiDataServer *server,
                                     UmiStore *out_store);
/**
 * Copy store into module-owned storage so callers keep ownership of their input values.
 */
UmiStatus umi_store_set(UmiStore *store,
                        const char *key,
                        const char *value);
/**
 * Provide the store get operation used by this module and its client applications.
 */
UmiStatus umi_store_get(const UmiStore *store,
                        const char *key,
                        char *value,
                        size_t value_capacity);
/**
 * Remove store while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_store_remove(UmiStore *store, const char *key);
/**
 * Return the number of records represented by store without changing their state.
 */
size_t umi_store_count(const UmiStore *store);

#ifdef __cplusplus
}
#endif

#endif
