/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/perspective_registry.h
 *
 * PURPOSE:
 *   Define task-oriented perspectives that select a default layout, panels and commands without confusing perspective, workspace, session and layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_PERSPECTIVE_REGISTRY_H
#define UMICOM_WORKBENCH_LAYOUT_PERSPECTIVE_REGISTRY_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_PERSPECTIVE_MAX_PANELS 32U
#define UMI_WORKBENCH_PERSPECTIVE_MAX_COMMANDS 64U

typedef struct UmiWorkbenchPerspectiveDefinition {
    uint32_t structure_size;
    char perspective_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char description[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char default_layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char panel_ids[UMI_WORKBENCH_PERSPECTIVE_MAX_PANELS]
                  [UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    size_t panel_count;
    char command_ids[UMI_WORKBENCH_PERSPECTIVE_MAX_COMMANDS]
                    [UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    size_t command_count;
    bool built_in;
    bool default_for_application;
    uint64_t revision;
} UmiWorkbenchPerspectiveDefinition;

typedef struct UmiWorkbenchPerspectiveRegistry {
    uint32_t structure_size;
    UmiWorkbenchPerspectiveDefinition
        perspectives[UMI_WORKBENCH_LAYOUT_MAX_PERSPECTIVES];
    size_t count;
    uint64_t revision;
} UmiWorkbenchPerspectiveRegistry;

void umi_workbench_perspective_registry_init(
    UmiWorkbenchPerspectiveRegistry *registry);

UmiStatus umi_workbench_perspective_definition_validate(
    const UmiWorkbenchPerspectiveDefinition *definition);

UmiStatus umi_workbench_perspective_registry_add(
    UmiWorkbenchPerspectiveRegistry *registry,
    const UmiWorkbenchPerspectiveDefinition *definition);

UmiStatus umi_workbench_perspective_registry_remove(
    UmiWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id);

const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_find(
    const UmiWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id);

const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_default_for_application(
    const UmiWorkbenchPerspectiveRegistry *registry,
    const char *owner_application_id);

const UmiWorkbenchPerspectiveDefinition *
umi_workbench_perspective_registry_at(
    const UmiWorkbenchPerspectiveRegistry *registry,
    size_t index);

UmiStatus umi_workbench_perspective_definition_add_panel(
    UmiWorkbenchPerspectiveDefinition *definition,
    const char *panel_id);

UmiStatus umi_workbench_perspective_definition_add_command(
    UmiWorkbenchPerspectiveDefinition *definition,
    const char *command_id);

#ifdef __cplusplus
}
#endif

#endif
