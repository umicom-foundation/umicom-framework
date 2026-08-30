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

void umi_workbench_panel_registry_init(
    UmiWorkbenchPanelRegistry *registry)
{
    if (registry == NULL) {
        return;
    }
    (void)memset(registry, 0, sizeof(*registry));
    registry->structure_size = sizeof(*registry);
    registry->revision = 1U;
}

UmiStatus umi_workbench_panel_definition_validate(
    const UmiWorkbenchPanelDefinition *definition)
{
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
    if (definition->default_region <
            UMI_WORKBENCH_LAYOUT_DOCK_CANVAS ||
        definition->default_region >
            UMI_WORKBENCH_LAYOUT_DOCK_FLOATING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (definition->minimum_size.width < 0 ||
        definition->minimum_size.height < 0 ||
        definition->preferred_size.width < 0 ||
        definition->preferred_size.height < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (definition->preferred_size.width > 0 &&
        definition->minimum_size.width >
            definition->preferred_size.width) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (definition->preferred_size.height > 0 &&
        definition->minimum_size.height >
            definition->preferred_size.height) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((definition->flags &
         (uint32_t)UMI_WORKBENCH_PANEL_SINGLETON) != 0U &&
        (definition->flags &
         (uint32_t)UMI_WORKBENCH_PANEL_MULTI_INSTANCE) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_panel_registry_add(
    UmiWorkbenchPanelRegistry *registry,
    const UmiWorkbenchPanelDefinition *definition)
{
    UmiStatus status;

    if (registry == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_panel_definition_validate(definition);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (registry->count >= UMI_WORKBENCH_LAYOUT_MAX_PANELS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
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

UmiStatus umi_workbench_panel_registry_remove(
    UmiWorkbenchPanelRegistry *registry,
    const char *panel_id)
{
    size_t index;

    if (registry == NULL ||
        !umi_workbench_layout_text_present(panel_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->panels[index].panel_id,
                panel_id) != 0) {
            continue;
        }
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

const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_find(
    const UmiWorkbenchPanelRegistry *registry,
    const char *panel_id)
{
    size_t index;

    if (registry == NULL ||
        !umi_workbench_layout_text_present(panel_id)) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->panels[index].panel_id,
                panel_id) == 0) {
            return &registry->panels[index];
        }
    }
    return NULL;
}

const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_at(
    const UmiWorkbenchPanelRegistry *registry,
    size_t index)
{
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }
    return &registry->panels[index];
}

size_t umi_workbench_panel_registry_count_owner(
    const UmiWorkbenchPanelRegistry *registry,
    const char *owner_application_id)
{
    size_t index;
    size_t count = 0U;

    if (registry == NULL ||
        !umi_workbench_layout_text_present(owner_application_id)) {
        return 0U;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->panels[index].owner_application_id,
                owner_application_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

size_t umi_workbench_panel_registry_count_category(
    const UmiWorkbenchPanelRegistry *registry,
    const char *category)
{
    size_t index;
    size_t count = 0U;

    if (registry == NULL ||
        !umi_workbench_layout_text_present(category)) {
        return 0U;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->panels[index].category,
                category) == 0) {
            count += 1U;
        }
    }
    return count;
}

bool umi_workbench_panel_definition_has_flag(
    const UmiWorkbenchPanelDefinition *definition,
    UmiWorkbenchPanelFlags flag)
{
    return definition != NULL &&
           (definition->flags & (uint32_t)flag) != 0U;
}
