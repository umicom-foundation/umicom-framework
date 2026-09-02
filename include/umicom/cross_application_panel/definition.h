/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/definition.h
 *
 * PURPOSE:
 *   Define cross-application panel definition state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_DEFINITION_H
#define UMICOM_CROSS_APPLICATION_PANEL_DEFINITION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel definition data shared with callers of this public contract.
 */
typedef struct UmiPanelDefinition {
    uint32_t structure_size;
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char application_id[UMI_PANEL_TEXT_CAPACITY];
    char title[UMI_PANEL_TEXT_CAPACITY];
    char description[UMI_PANEL_DESCRIPTION_CAPACITY];
    char component_id[UMI_PANEL_TEXT_CAPACITY];
    char default_channel_id[UMI_PANEL_TEXT_CAPACITY];
    char category[UMI_PANEL_TEXT_CAPACITY];
    bool singleton;
    bool context_aware;
    bool enabled;
    uint64_t revision;
} UmiPanelDefinition;
/**
 * Represent the panel definition store data shared with callers of this public contract.
 */
typedef struct UmiPanelDefinitionStore { UmiPanelDefinition items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelDefinitionStore;
/**
 * Initialise panel definition from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_definition_init(UmiPanelDefinition *record);
/**
 * Check that panel definition satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_definition_validate(const UmiPanelDefinition *record);
/**
 * Initialise panel definition store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_definition_store_init(UmiPanelDefinitionStore *store);
/**
 * Provide the panel definition store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_definition_store_put(UmiPanelDefinitionStore *store,const UmiPanelDefinition *record);
/**
 * Remove panel definition store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_definition_store_remove(UmiPanelDefinitionStore *store,const char *identity);
/**
 * Find panel definition store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelDefinition *umi_panel_definition_store_find(UmiPanelDefinitionStore *store,const char *identity);
/**
 * Provide the panel definition store find const operation used by this module and its
 * client applications.
 */
const UmiPanelDefinition *umi_panel_definition_store_find_const(const UmiPanelDefinitionStore *store,const char *identity);
/**
 * Provide the panel definition store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_definition_store_snapshot(const UmiPanelDefinitionStore *store,UmiPanelDefinition *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
