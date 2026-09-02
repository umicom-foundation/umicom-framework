/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/perspective_registry.c
 *
 * PURPOSE:
 *   Implement task-oriented perspective registration while preserving the distinction between perspective, workspace, session and physical layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/perspective_registry.h"

#include <string.h>

#include "internal.h"

/*
 * Provide the string array contains operation used by this module and its client
 * applications.
 */
static bool string_array_contains(
    char values[][UMI_WORKBENCH_LAYOUT_ID_CAPACITY],
    size_t count,
    const char *value)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(values[index], value) == 0) {
            return true;
        }
    }
    return false;
}

/*
 * Initialise workbench perspective registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_perspective_registry_init(
    UmiWorkbenchPerspectiveRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return;
    }
    (void)memset(registry, 0, sizeof(*registry));
    registry->structure_size = sizeof(*registry);
    registry->revision = 1U;
}

/*
 * Check that workbench perspective definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_perspective_definition_validate(
    const UmiWorkbenchPerspectiveDefinition *definition)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        definition->structure_size < sizeof(*definition) ||
        !umi_workbench_layout_text_present(
            definition->perspective_id) ||
        !umi_workbench_layout_text_present(
            definition->display_name) ||
        !umi_workbench_layout_text_present(
            definition->owner_application_id) ||
        !umi_workbench_layout_text_present(
            definition->default_layout_id) ||
        definition->panel_count >
            UMI_WORKBENCH_PERSPECTIVE_MAX_PANELS ||
        definition->command_count >
            UMI_WORKBENCH_PERSPECTIVE_MAX_COMMANDS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->panel_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_text_present(
                definition->panel_ids[index])) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->command_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_text_present(
                definition->command_ids[index])) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Add workbench perspective registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_perspective_registry_add(
    UmiWorkbenchPerspectiveRegistry *registry,
    const UmiWorkbenchPerspectiveDefinition *definition)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_perspective_definition_validate(
        definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >=
        UMI_WORKBENCH_LAYOUT_MAX_PERSPECTIVES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_perspective_registry_find(
            registry, definition->perspective_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    registry->perspectives[registry->count] = *definition;
    registry->perspectives[registry->count].structure_size =
        sizeof(registry->perspectives[registry->count]);
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench perspective registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_perspective_registry_remove(
    UmiWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(perspective_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->perspectives[index].perspective_id,
                perspective_id) != 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->perspectives[index].built_in) {
            return UMI_STATUS_PERMISSION_DENIED;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (; index + 1U < registry->count; ++index) {
            registry->perspectives[index] =
                registry->perspectives[index + 1U];
        }
        registry->count -= 1U;
        (void)memset(
            &registry->perspectives[registry->count],
            0,
            sizeof(registry->perspectives[registry->count]));
        registry->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find workbench perspective registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_find(
    const UmiWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(perspective_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->perspectives[index].perspective_id,
                perspective_id) == 0) {
            return &registry->perspectives[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench perspective registry default for application operation used by
 * this module and its client applications.
 */
const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_default_for_application(
    const UmiWorkbenchPerspectiveRegistry *registry,
    const char *owner_application_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(owner_application_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->perspectives[index].default_for_application &&
            strcmp(
                registry->perspectives[index].owner_application_id,
                owner_application_id) == 0) {
            return &registry->perspectives[index];
        }
    }
    return NULL;
}

/*
 * Find workbench perspective registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_at(
    const UmiWorkbenchPerspectiveRegistry *registry,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }
    return &registry->perspectives[index];
}

/*
 * Provide the workbench perspective definition add panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_perspective_definition_add_panel(
    UmiWorkbenchPerspectiveDefinition *definition,
    const char *panel_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        !umi_workbench_layout_text_present(panel_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (definition->panel_count >=
        UMI_WORKBENCH_PERSPECTIVE_MAX_PANELS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (string_array_contains(
            definition->panel_ids,
            definition->panel_count,
            panel_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    status = umi_workbench_layout_copy_text(
        definition->panel_ids[definition->panel_count],
        sizeof(definition->panel_ids[definition->panel_count]),
        panel_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        definition->panel_count += 1U;
        definition->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench perspective definition add command operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_perspective_definition_add_command(
    UmiWorkbenchPerspectiveDefinition *definition,
    const char *command_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        !umi_workbench_layout_text_present(command_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (definition->command_count >=
        UMI_WORKBENCH_PERSPECTIVE_MAX_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (string_array_contains(
            definition->command_ids,
            definition->command_count,
            command_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    status = umi_workbench_layout_copy_text(
        definition->command_ids[definition->command_count],
        sizeof(definition->command_ids[definition->command_count]),
        command_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        definition->command_count += 1U;
        definition->revision += 1U;
    }
    return status;
}
