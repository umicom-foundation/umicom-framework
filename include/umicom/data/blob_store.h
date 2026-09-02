/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/blob_store.h
 *
 * PURPOSE:
 *   Declare binary-object persistence over a UmiStore using deterministic hexadecimal encoding and namespaced identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_BLOB_STORE_H
#define UMICOM_DATA_BLOB_STORE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/data/store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the blob store data shared with callers of this public contract.
 */
typedef struct UmiBlobStore {
    UmiStore store;
    char prefix[160];
} UmiBlobStore;

/**
 * Initialise blob store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_blob_store_init(UmiBlobStore *blob_store,
                              const UmiStore *store,
                              const char *namespace_name);
/**
 * Provide the blob store put operation used by this module and its client applications.
 */
UmiStatus umi_blob_store_put(UmiBlobStore *blob_store,
                             const char *blob_id,
                             const void *bytes,
                             size_t size);
/**
 * Provide the blob store get operation used by this module and its client applications.
 */
UmiStatus umi_blob_store_get(const UmiBlobStore *blob_store,
                             const char *blob_id,
                             unsigned char **out_bytes,
                             size_t *out_size);
/**
 * Remove blob store while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_blob_store_remove(UmiBlobStore *blob_store,
                                const char *blob_id);
/**
 * Provide the blob store free operation used by this module and its client applications.
 */
void umi_blob_store_free(void *bytes);

#ifdef __cplusplus
}
#endif

#endif
