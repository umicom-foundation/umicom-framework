/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/blob_store.c
 *
 * PURPOSE:
 *   Implement namespaced binary storage through hexadecimal encoding, retrieval, decoding and explicit ownership cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/blob_store.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/data_internal.h"

/* Provide the blob key operation used by this module and its client applications. */
static UmiStatus blob_key(const UmiBlobStore *store,
                          const char *blob_id,
                          char *out_key,
                          size_t capacity)
{
    return umi_data_key(out_key, capacity, store->prefix, blob_id);
}

/*
 * Initialise blob store from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_blob_store_init(UmiBlobStore *blob_store,
                              const UmiStore *store,
                              const char *namespace_name)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (blob_store == NULL || store == NULL || namespace_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    blob_store->store = *store;
    written = snprintf(blob_store->prefix,
                       sizeof(blob_store->prefix),
                       "blob.%s.",
                       namespace_name);
    return written < 0 || (size_t)written >= sizeof(blob_store->prefix)
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Provide the blob store put operation used by this module and its client applications. */
UmiStatus umi_blob_store_put(UmiBlobStore *blob_store,
                             const char *blob_id,
                             const void *bytes,
                             size_t size)
{
    char key[320];
    char *encoded = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (blob_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = blob_key(blob_store, blob_id, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_hex_encode(bytes, size, &encoded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_store_set(&blob_store->store, key, encoded);
    }
    free(encoded);
    return status;
}

/* Provide the blob store get operation used by this module and its client applications. */
UmiStatus umi_blob_store_get(const UmiBlobStore *blob_store,
                             const char *blob_id,
                             unsigned char **out_bytes,
                             size_t *out_size)
{
    char key[320];
    char *encoded;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (blob_store == NULL || out_bytes == NULL || out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = blob_key(blob_store, blob_id, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    encoded = (char *)malloc(8192U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_store_get(&blob_store->store,
                           key,
                           encoded,
                           8192U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_hex_decode(encoded, out_bytes, out_size);
    }
    free(encoded);
    return status;
}

/* Remove blob store while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_blob_store_remove(UmiBlobStore *blob_store,
                                const char *blob_id)
{
    char key[320];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (blob_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = blob_key(blob_store, blob_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_remove(&blob_store->store, key)
        : status;
}

/* Provide the blob store free operation used by this module and its client applications. */
void umi_blob_store_free(void *bytes)
{
    free(bytes);
}
