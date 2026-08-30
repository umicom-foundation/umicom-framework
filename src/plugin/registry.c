/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/registry.c
 *
 * PURPOSE:
 *   Implement the authoritative plug-in catalogue with validated lifecycle
 *   transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/registry.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/platform/threading.h"
struct UmiPluginRegistry { UmiPluginRecord items[UMI_PLUGIN_REGISTRY_MAX]; size_t count; UmiMutex *mutex; };
static size_t plugin_find(const UmiPluginRegistry *r, const char *id)
{ size_t i; for (i = 0U; i < r->count; ++i) if (strcmp(r->items[i].manifest.plugin_id, id) == 0) return i; return SIZE_MAX; }
UmiStatus umi_plugin_registry_create(UmiPluginRegistry **out_registry)
{
    UmiPluginRegistry *r; UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; r = (UmiPluginRegistry *)calloc(1U, sizeof(*r)); if (r == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&r->mutex); if (status != UMI_STATUS_OK) { free(r); return status; }
    *out_registry = r; return UMI_STATUS_OK;
}
void umi_plugin_registry_destroy(UmiPluginRegistry *r) { if (r != NULL) { umi_mutex_destroy(r->mutex); free(r); } }
UmiStatus umi_plugin_registry_add(UmiPluginRegistry *r, const UmiPluginManifest *manifest, const char *manifest_path)
{
    UmiPluginRecord *record; char reason[128]; UmiStatus status;
    if (r == NULL || manifest == NULL || manifest_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_plugin_manifest_validate(manifest, reason, sizeof(reason)); if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    if (plugin_find(r, manifest->plugin_id) != SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    if (r->count >= UMI_PLUGIN_REGISTRY_MAX || strlen(manifest_path) >= UMI_PATH_CAPACITY) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    record = &r->items[r->count++]; (void)memset(record, 0, sizeof(*record)); record->manifest = *manifest; (void)snprintf(record->manifest_path, sizeof(record->manifest_path), "%s", manifest_path); record->state = UMI_PLUGIN_DISCOVERED; record->enabled = 1;
    (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_plugin_registry_get(const UmiPluginRegistry *r, const char *plugin_id, UmiPluginRecord *out_record)
{
    size_t index; UmiStatus status;
    if (r == NULL || plugin_id == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_plugin_registry_set_enabled(UmiPluginRegistry *r, const char *plugin_id, int enabled)
{
    size_t index; UmiStatus status;
    if (r == NULL || plugin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    r->items[index].enabled = enabled != 0; if (!enabled) r->items[index].state = UMI_PLUGIN_DISABLED;
    (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_plugin_registry_transition(UmiPluginRegistry *r, const char *plugin_id, UmiPluginState state)
{
    size_t index; UmiStatus status;
    if (r == NULL || plugin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    if (!umi_plugin_state_transition_valid(r->items[index].state, state)) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_INVALID_STATE; }
    r->items[index].state = state; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
size_t umi_plugin_registry_count(const UmiPluginRegistry *r)
{ size_t n = 0U; if (r != NULL && umi_mutex_lock(r->mutex) == UMI_STATUS_OK) { n = r->count; (void)umi_mutex_unlock(r->mutex); } return n; }
UmiStatus umi_plugin_registry_at(const UmiPluginRegistry *r, size_t index, UmiPluginRecord *out_record)
{
    UmiStatus status; if (r == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); if (status != UMI_STATUS_OK) return status;
    if (index >= r->count) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
