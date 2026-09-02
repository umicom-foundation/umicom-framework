/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_point.c
 *
 * PURPOSE:
 *   Implement typed extension-point registration and candidate validation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/extension_point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiPluginExtensionPointRegistry {
    UmiPluginExtensionPoint items[UMI_PLUGIN_EXTENSION_POINT_MAX];
    size_t count;
};

/* Find point while leaving the underlying catalogue or model owned by this module. */
static size_t point_find(const UmiPluginExtensionPointRegistry *registry,
                         const char *extension_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].extension_id, extension_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise plugin extension point registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_plugin_extension_point_registry_create(
    UmiPluginExtensionPointRegistry **out_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = (UmiPluginExtensionPointRegistry *)calloc(1U, sizeof(**out_registry));
    return *out_registry != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by plugin extension point registry so the same storage can
 * be reused safely.
 */
void umi_plugin_extension_point_registry_destroy(
    UmiPluginExtensionPointRegistry *registry)
{
    free(registry);
}

/*
 * Add plugin extension point registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_plugin_extension_point_registry_add(
    UmiPluginExtensionPointRegistry *registry,
    const UmiPluginExtensionPoint *point)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || point == NULL || point->extension_id[0] == '\0' ||
        point->owner_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (point_find(registry, point->extension_id) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_PLUGIN_EXTENSION_POINT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->items[registry->count++] = *point;
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin extension point registry get operation used by this module and its
 * client applications.
 */
UmiStatus umi_plugin_extension_point_registry_get(
    const UmiPluginExtensionPointRegistry *registry,
    const char *extension_id,
    UmiPluginExtensionPoint *out_point)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || extension_id == NULL || out_point == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = point_find(registry, extension_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_point = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin extension point validate contribution operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_point_validate_contribution(
    const UmiPluginExtensionPointRegistry *registry,
    const UmiPluginContributionRegistry *contributions,
    const UmiPluginContribution *candidate,
    char *out_reason,
    size_t reason_capacity)
{
    UmiPluginExtensionPoint point;
    size_t existing;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contributions == NULL || candidate == NULL ||
        out_reason == NULL || reason_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_plugin_extension_point_registry_get(registry,
                                                candidate->contribution_type,
                                                &point) != UMI_STATUS_OK) {
        (void)snprintf(out_reason, reason_capacity,
                       "unknown extension point: %s", candidate->contribution_type);
        return UMI_STATUS_NOT_FOUND;
    }
    existing = umi_plugin_contribution_registry_count_type(
        contributions, candidate->contribution_type);
    /* Apply this branch only when its contract condition is satisfied. */
    if ((point.cardinality == UMI_PLUGIN_EXTENSION_ZERO_OR_ONE ||
         point.cardinality == UMI_PLUGIN_EXTENSION_ONE) && existing > 0U) {
        (void)snprintf(out_reason, reason_capacity,
                       "extension point %s accepts one contribution",
                       candidate->contribution_type);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate->target[0] == '\0' && point.schema[0] != '\0') {
        (void)snprintf(out_reason, reason_capacity,
                       "extension point %s requires a target",
                       candidate->contribution_type);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(out_reason, reason_capacity, "contribution accepted");
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by plugin extension point registry without
 * changing their state.
 */
size_t umi_plugin_extension_point_registry_count(
    const UmiPluginExtensionPointRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
