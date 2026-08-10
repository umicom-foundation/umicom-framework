/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/store.h
 *
 * PURPOSE:
 *   Define a provider-neutral key/value store contract and a Data Server adapter used by repositories, caches and durable messaging.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_store_from_data_server(UmiDataServer *server,
                                     UmiStore *out_store);
UmiStatus umi_store_set(UmiStore *store,
                        const char *key,
                        const char *value);
UmiStatus umi_store_get(const UmiStore *store,
                        const char *key,
                        char *value,
                        size_t value_capacity);
UmiStatus umi_store_remove(UmiStore *store, const char *key);
size_t umi_store_count(const UmiStore *store);

#ifdef __cplusplus
}
#endif

#endif
