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

typedef struct UmiBlobStore {
    UmiStore store;
    char prefix[160];
} UmiBlobStore;

UmiStatus umi_blob_store_init(UmiBlobStore *blob_store,
                              const UmiStore *store,
                              const char *namespace_name);
UmiStatus umi_blob_store_put(UmiBlobStore *blob_store,
                             const char *blob_id,
                             const void *bytes,
                             size_t size);
UmiStatus umi_blob_store_get(const UmiBlobStore *blob_store,
                             const char *blob_id,
                             unsigned char **out_bytes,
                             size_t *out_size);
UmiStatus umi_blob_store_remove(UmiBlobStore *blob_store,
                                const char *blob_id);
void umi_blob_store_free(void *bytes);

#ifdef __cplusplus
}
#endif

#endif
