/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/panel_registry.c
 *
 * PURPOSE:
 *   Implement reusable panel contribution registration, validation, ownership queries and singleton policy for every Umicom workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/panel_registry.h"

#include <string.h>

#include "internal.h"

/*
 * Initialise workbench panel registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_panel_registry_init(
    UmiWorkbenchPanelRegistry *registry)
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
 * Check that workbench panel definition satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_panel_definition_validate(
    const UmiWorkbenchPanelDefinition *definition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        definition->structure_size < sizeof(*definition) ||
        !umi_workbench_layout_text_present(definition->panel_id) ||
        !umi_workbench_layout_text_present(
            definition->display_name) ||
        !umi_workbench_layout_text_present(
            definition->owner_application_id) ||
        !umi_workbench_layout_text_present(definition->category)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->default_region <
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS ||
        definition->default_region >
            UMI_WORKBENCH_LAYOUT_DOCK_FLOATING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->minimum_size.width < 0 ||
        definition->minimum_size.height < 0 ||
        definition->preferred_size.width < 0 ||
        definition->preferred_size.height < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->preferred_size.width > 0 &&
        definition->minimum_size.width >
            definition->preferred_size.width) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->preferred_size.height > 0 &&
        definition->minimum_size.height >
            definition->preferred_size.height) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((definition->flags &
         (uint32_t)UMI_WORKBENCH_PANEL_SINGLETON) != 0U &&
        (definition->flags &
         (uint32_t)UMI_WORKBENCH_PANEL_MULTI_INSTANCE) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Add workbench panel registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_panel_registry_add(
    UmiWorkbenchPanelRegistry *registry,
    const UmiWorkbenchPanelDefinition *definition)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_panel_definition_validate(definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_WORKBENCH_LAYOUT_MAX_PANELS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_panel_registry_find(
            registry, definition->panel_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    registry->panels[registry->count] = *definition;
    registry->panels[registry->count].structure_size =
        sizeof(registry->panels[registry->count]);
    registry->count += 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench panel registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_panel_registry_remove(
    UmiWorkbenchPanelRegistry *registry,
    const char *panel_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(panel_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->panels[index].panel_id,
                panel_id) != 0) {
            continue;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (; index + 1U < registry->count; ++index) {
            registry->panels[index] =
                registry->panels[index + 1U];
        }
        registry->count -= 1U;
        (void)memset(
            &registry->panels[registry->count],
            0,
            sizeof(registry->panels[registry->count]));
        registry->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find workbench panel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_find(
    const UmiWorkbenchPanelRegistry *registry,
    const char *panel_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(panel_id)) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->panels[index].panel_id,
                panel_id) == 0) {
            return &registry->panels[index];
        }
    }
    return NULL;
}

/*
 * Find workbench panel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_at(
    const UmiWorkbenchPanelRegistry *registry,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }
    return &registry->panels[index];
}

/*
 * Provide the workbench panel registry count owner operation used by this module and its
 * client applications.
 */
size_t umi_workbench_panel_registry_count_owner(
    const UmiWorkbenchPanelRegistry *registry,
    const char *owner_application_id)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(owner_application_id)) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->panels[index].owner_application_id,
                owner_application_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench panel registry count category operation used by this module and
 * its client applications.
 */
size_t umi_workbench_panel_registry_count_category(
    const UmiWorkbenchPanelRegistry *registry,
    const char *category)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        !umi_workbench_layout_text_present(category)) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->panels[index].category,
                category) == 0) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the workbench panel definition has flag operation used by this module and its
 * client applications.
 */
bool umi_workbench_panel_definition_has_flag(
    const UmiWorkbenchPanelDefinition *definition,
    UmiWorkbenchPanelFlags flag)
{
    return definition != NULL &&
           (definition->flags & (uint32_t)flag) != 0U;
}
