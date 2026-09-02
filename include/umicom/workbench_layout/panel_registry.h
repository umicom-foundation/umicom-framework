/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/panel_registry.h
 *
 * PURPOSE:
 *   Describe reusable panel contributions, placement policy and singleton constraints independently of the active GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_PANEL_REGISTRY_H
#define UMICOM_WORKBENCH_LAYOUT_PANEL_REGISTRY_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench panel flags values accepted by this public contract.
 */
typedef enum UmiWorkbenchPanelFlags {
    UMI_WORKBENCH_PANEL_SINGLETON = 1U << 0,
    UMI_WORKBENCH_PANEL_MULTI_INSTANCE = 1U << 1,
    UMI_WORKBENCH_PANEL_CLOSABLE = 1U << 2,
    UMI_WORKBENCH_PANEL_MOVABLE = 1U << 3,
    UMI_WORKBENCH_PANEL_FLOATABLE = 1U << 4,
    UMI_WORKBENCH_PANEL_REQUIRES_WORKSPACE = 1U << 5,
    UMI_WORKBENCH_PANEL_REQUIRES_TRUST = 1U << 6,
    UMI_WORKBENCH_PANEL_PERSIST_STATE = 1U << 7
} UmiWorkbenchPanelFlags;

/**
 * Represent the workbench panel definition data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchPanelDefinition {
    uint32_t structure_size;
    char panel_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char description[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char icon_resource_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char required_capability[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutDockRegion default_region;
    UmiWorkbenchLayoutSize minimum_size;
    UmiWorkbenchLayoutSize preferred_size;
    uint32_t flags;
    uint64_t revision;
} UmiWorkbenchPanelDefinition;

/**
 * Represent the workbench panel registry data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchPanelRegistry {
    uint32_t structure_size;
    UmiWorkbenchPanelDefinition panels[UMI_WORKBENCH_LAYOUT_MAX_PANELS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchPanelRegistry;

/**
 * Initialise workbench panel registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_panel_registry_init(
    UmiWorkbenchPanelRegistry *registry);

/**
 * Check that workbench panel definition satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_panel_definition_validate(
    const UmiWorkbenchPanelDefinition *definition);

/**
 * Add workbench panel registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_panel_registry_add(
    UmiWorkbenchPanelRegistry *registry,
    const UmiWorkbenchPanelDefinition *definition);

/**
 * Remove workbench panel registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_panel_registry_remove(
    UmiWorkbenchPanelRegistry *registry,
    const char *panel_id);

/**
 * Find workbench panel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_find(
    const UmiWorkbenchPanelRegistry *registry,
    const char *panel_id);

/**
 * Find workbench panel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchPanelDefinition *
umi_workbench_panel_registry_at(
    const UmiWorkbenchPanelRegistry *registry,
    size_t index);

/**
 * Provide the workbench panel registry count owner operation used by this module and its
 * client applications.
 */
size_t umi_workbench_panel_registry_count_owner(
    const UmiWorkbenchPanelRegistry *registry,
    const char *owner_application_id);

/**
 * Provide the workbench panel registry count category operation used by this module and
 * its client applications.
 */
size_t umi_workbench_panel_registry_count_category(
    const UmiWorkbenchPanelRegistry *registry,
    const char *category);

/**
 * Provide the workbench panel definition has flag operation used by this module and its
 * client applications.
 */
bool umi_workbench_panel_definition_has_flag(
    const UmiWorkbenchPanelDefinition *definition,
    UmiWorkbenchPanelFlags flag);

#ifdef __cplusplus
}
#endif

#endif
