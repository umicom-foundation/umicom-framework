/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/workspace_trust.c
 *
 * PURPOSE:
 *   Implement canonical-path trust decisions with explicit decision provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/workspace_trust.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/platform/threading.h"
struct UmiWorkspaceTrustStore { UmiWorkspaceTrustRecord items[UMI_WORKSPACE_TRUST_MAX]; size_t count; UmiMutex *mutex; };
static size_t trust_find(const UmiWorkspaceTrustStore *store, const char *path)
{ size_t i; for (i = 0U; i < store->count; ++i) if (strcmp(store->items[i].path, path) == 0) return i; return SIZE_MAX; }
UmiStatus umi_workspace_trust_store_create(UmiWorkspaceTrustStore **out_store)
{
    UmiWorkspaceTrustStore *store; UmiStatus status;
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL; store = (UmiWorkspaceTrustStore *)calloc(1U, sizeof(*store)); if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&store->mutex); if (status != UMI_STATUS_OK) { free(store); return status; }
    *out_store = store; return UMI_STATUS_OK;
}
void umi_workspace_trust_store_destroy(UmiWorkspaceTrustStore *store)
{ if (store != NULL) { umi_mutex_destroy(store->mutex); free(store); } }
UmiStatus umi_workspace_trust_store_set(UmiWorkspaceTrustStore *store, const char *path, UmiWorkspaceTrustLevel level, const char *decided_by, uint64_t decided_at_ns)
{
    char normalised[UMI_PATH_CAPACITY]; size_t index; UmiWorkspaceTrustRecord *record; UmiStatus status;
    if (store == NULL || path == NULL || decided_by == NULL || level < UMI_WORKSPACE_UNTRUSTED || level > UMI_WORKSPACE_TRUSTED) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_path_normalise(path, normalised, sizeof(normalised)); if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(store->mutex); if (status != UMI_STATUS_OK) return status;
    index = trust_find(store, normalised);
    if (index == SIZE_MAX) { if (store->count >= UMI_WORKSPACE_TRUST_MAX) { (void)umi_mutex_unlock(store->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; } index = store->count++; }
    record = &store->items[index]; (void)memset(record, 0, sizeof(*record));
    (void)snprintf(record->path, sizeof(record->path), "%s", normalised);
    (void)snprintf(record->decided_by, sizeof(record->decided_by), "%s", decided_by);
    record->level = level; record->decided_at_ns = decided_at_ns;
    (void)umi_mutex_unlock(store->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_workspace_trust_store_get(const UmiWorkspaceTrustStore *store, const char *path, UmiWorkspaceTrustRecord *out_record)
{
    char normalised[UMI_PATH_CAPACITY]; size_t index; UmiStatus status;
    if (store == NULL || path == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_path_normalise(path, normalised, sizeof(normalised)); if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(store->mutex); if (status != UMI_STATUS_OK) return status;
    index = trust_find(store, normalised); if (index == SIZE_MAX) { (void)umi_mutex_unlock(store->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = store->items[index]; (void)umi_mutex_unlock(store->mutex); return UMI_STATUS_OK;
}
size_t umi_workspace_trust_store_count(const UmiWorkspaceTrustStore *store)
{ size_t n = 0U; if (store != NULL && umi_mutex_lock(store->mutex) == UMI_STATUS_OK) { n = store->count; (void)umi_mutex_unlock(store->mutex); } return n; }
