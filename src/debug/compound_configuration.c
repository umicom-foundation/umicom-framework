/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/compound_configuration.c
 *
 * PURPOSE:
 *   Implement validated compound debugger launch plans with deterministic
 *   ordering and required-configuration safety.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/compound_configuration.h"

#include <stdlib.h>
#include <string.h>

struct UmiDebugCompoundConfiguration {
    UmiDebugCompoundDescriptor descriptor;
    UmiDebugCompoundEntry *entries;
    size_t count;
    size_t capacity;
    UmiDebugCompoundState state;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(const UmiDebugCompoundConfiguration *compound,
                         const char *configuration_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || configuration_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < compound->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(compound->entries[index].configuration_id,
                   configuration_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiDebugCompoundConfiguration *compound,
                                 size_t required)
{
    size_t capacity;
    UmiDebugCompoundEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= compound->capacity) return UMI_STATUS_OK;
    capacity = compound->capacity > 0U ? compound->capacity : 4U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugCompoundEntry *)realloc(
        compound->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    compound->entries = replacement;
    compound->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the compare entries operation used by this module and its client applications. */
static int compare_entries(const void *left, const void *right)
{
    const UmiDebugCompoundEntry *a = (const UmiDebugCompoundEntry *)left;
    const UmiDebugCompoundEntry *b = (const UmiDebugCompoundEntry *)right;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->order < b->order) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->order > b->order) return 1;
    return strcmp(a->configuration_id, b->configuration_id);
}

/*
 * Initialise debug compound configuration from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_compound_configuration_create(
    UmiDebugCompoundConfiguration **out_compound)
{
    UmiDebugCompoundConfiguration *compound;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_compound == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_compound = NULL;
    compound = (UmiDebugCompoundConfiguration *)calloc(1U,
                                                        sizeof(*compound));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    compound->revision = 1U;
    *out_compound = compound;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug compound configuration so the same storage can be
 * reused safely.
 */
void umi_debug_compound_configuration_destroy(
    UmiDebugCompoundConfiguration *compound)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL) return;
    free(compound->entries);
    free(compound);
}

/*
 * Provide the debug compound configuration begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_begin(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        (descriptor->launch_mode != UMI_DEBUG_COMPOUND_LAUNCH_PARALLEL &&
         descriptor->launch_mode != UMI_DEBUG_COMPOUND_LAUNCH_SEQUENTIAL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    compound->descriptor = *descriptor;
    compound->descriptor.stop_all = descriptor->stop_all != 0;
    compound->count = 0U;
    compound->state = UMI_DEBUG_COMPOUND_CONFIGURING;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Add debug compound configuration only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_compound_configuration_add(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundEntry *entry)
{
    UmiDebugCompoundEntry stored;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || entry == NULL ||
        compound->state != UMI_DEBUG_COMPOUND_CONFIGURING ||
        entry->struct_size != (uint32_t)sizeof(*entry) ||
        entry->api_version != UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION ||
        !terminated(entry->configuration_id,
                    sizeof(entry->configuration_id)) ||
        entry->configuration_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_entry(compound, entry->configuration_id) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (entry->required && !entry->enabled) return UMI_STATUS_INVALID_ARGUMENT;
    status = reserve_entries(compound, compound->count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    stored = *entry;
    stored.enabled = entry->enabled != 0;
    stored.required = entry->required != 0;
    stored.launchable = 0;
    compound->entries[compound->count++] = stored;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove debug compound configuration while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_compound_configuration_remove(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || configuration_id == NULL ||
        configuration_id[0] == '\0' ||
        compound->state != UMI_DEBUG_COMPOUND_CONFIGURING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(compound, configuration_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (compound->entries[index].required) return UMI_STATUS_PERMISSION_DENIED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < compound->count) {
        (void)memmove(&compound->entries[index], &compound->entries[index + 1U],
                      (compound->count - index - 1U) *
                          sizeof(*compound->entries));
    }
    --compound->count;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the debug compound configuration set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_compound_configuration_set_enabled(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id,
    int enabled)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || configuration_id == NULL ||
        configuration_id[0] == '\0' ||
        compound->state != UMI_DEBUG_COMPOUND_CONFIGURING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(compound, configuration_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!enabled && compound->entries[index].required) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    compound->entries[index].enabled = enabled != 0;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the debug compound configuration finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_finalize(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters)
{
    size_t index;
    size_t launchable_count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || configurations == NULL || adapters == NULL ||
        compound->state != UMI_DEBUG_COMPOUND_CONFIGURING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (compound->count == 0U) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < compound->count; ++index) {
        UmiDebugLaunchConfigurationSnapshot launch;
        UmiDebugAdapterProfile adapter;
        UmiStatus status;
        compound->entries[index].launchable = 0;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!compound->entries[index].enabled) continue;
        status = umi_debug_launch_configuration_registry_find(
            configurations, compound->entries[index].configuration_id,
            &launch);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_debug_adapter_profile_registry_find(
                adapters, launch.adapter, &adapter);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && adapter.enabled &&
            adapter.supports_launch && launch.program[0] != '\0') {
            compound->entries[index].launchable = 1;
            ++launchable_count;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (compound->entries[index].required) {
            return status == UMI_STATUS_OK ? UMI_STATUS_UNAVAILABLE : status;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (launchable_count == 0U) return UMI_STATUS_UNAVAILABLE;
    qsort(compound->entries, compound->count, sizeof(*compound->entries),
          compare_entries);
    compound->state = UMI_DEBUG_COMPOUND_READY;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the debug compound configuration cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_cancel(
    UmiDebugCompoundConfiguration *compound)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (compound->state == UMI_DEBUG_COMPOUND_EMPTY) {
        return UMI_STATUS_INVALID_STATE;
    }
    compound->state = UMI_DEBUG_COMPOUND_CANCELLED;
    compound->revision = next_revision(compound->revision);
    return UMI_STATUS_OK;
}

/*
 * Find debug compound configuration while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_compound_configuration_at(
    const UmiDebugCompoundConfiguration *compound, size_t index,
    UmiDebugCompoundEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= compound->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = compound->entries[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug compound configuration snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_snapshot(
    const UmiDebugCompoundConfiguration *compound,
    UmiDebugCompoundSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compound == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION;
    out_snapshot->state = compound->state;
    out_snapshot->launch_mode = compound->descriptor.launch_mode;
    out_snapshot->entry_count = compound->count;
    out_snapshot->revision = compound->revision;
    out_snapshot->stop_all = compound->descriptor.stop_all;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < compound->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (compound->entries[index].enabled) ++out_snapshot->enabled_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (compound->entries[index].launchable) ++out_snapshot->launchable_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (compound->entries[index].required) ++out_snapshot->required_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug compound configuration without
 * changing their state.
 */
size_t umi_debug_compound_configuration_count(
    const UmiDebugCompoundConfiguration *compound)
{
    return compound != NULL ? compound->count : 0U;
}
