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
/* Find plugin while leaving the underlying catalogue or model owned by this module. */
static size_t plugin_find(const UmiPluginRegistry *r, const char *id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < r->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(r->items[i].manifest.plugin_id, id) == 0) return i; return SIZE_MAX; }
/*
 * Initialise plugin registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_registry_create(UmiPluginRegistry **out_registry)
{
    UmiPluginRegistry *r; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; r = (UmiPluginRegistry *)calloc(1U, sizeof(*r)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(r); return status; }
    *out_registry = r; return UMI_STATUS_OK;
}
/* Release or reset state held by plugin registry so the same storage can be reused safely. */
void umi_plugin_registry_destroy(UmiPluginRegistry *r) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r != NULL) { umi_mutex_destroy(r->mutex); free(r); } }
/* Add plugin registry only after its inputs and available capacity have been checked. */
UmiStatus umi_plugin_registry_add(UmiPluginRegistry *r, const UmiPluginManifest *manifest, const char *manifest_path)
{
    UmiPluginRecord *record; char reason[128]; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (r == NULL || manifest == NULL || manifest_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_plugin_manifest_validate(manifest, reason, sizeof(reason)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (plugin_find(r, manifest->plugin_id) != SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (r->count >= UMI_PLUGIN_REGISTRY_MAX || strlen(manifest_path) >= UMI_PATH_CAPACITY) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    record = &r->items[r->count++]; (void)memset(record, 0, sizeof(*record)); record->manifest = *manifest; (void)snprintf(record->manifest_path, sizeof(record->manifest_path), "%s", manifest_path); record->state = UMI_PLUGIN_DISCOVERED; record->enabled = 1;
    (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the plugin registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_get(const UmiPluginRegistry *r, const char *plugin_id, UmiPluginRecord *out_record)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (r == NULL || plugin_id == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the plugin registry set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_set_enabled(UmiPluginRegistry *r, const char *plugin_id, int enabled)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (r == NULL || plugin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    r->items[index].enabled = enabled != 0; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!enabled) r->items[index].state = UMI_PLUGIN_DISABLED;
    (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the plugin registry transition operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_registry_transition(UmiPluginRegistry *r, const char *plugin_id, UmiPluginState state)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (r == NULL || plugin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = plugin_find(r, plugin_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!umi_plugin_state_transition_valid(r->items[index].state, state)) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_INVALID_STATE; }
    r->items[index].state = state; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by plugin registry without changing their
 * state.
 */
size_t umi_plugin_registry_count(const UmiPluginRegistry *r)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r != NULL && umi_mutex_lock(r->mutex) == UMI_STATUS_OK) { n = r->count; (void)umi_mutex_unlock(r->mutex); } return n; }
/*
 * Find plugin registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_plugin_registry_at(const UmiPluginRegistry *r, size_t index, UmiPluginRecord *out_record)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (r == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(r->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= r->count) { (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = r->items[index]; (void)umi_mutex_unlock(r->mutex); return UMI_STATUS_OK;
}
