/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/contribution.h
 *
 * PURPOSE:
 *   Define cross-application panel contribution state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_CONTRIBUTION_H
#define UMICOM_CROSS_APPLICATION_PANEL_CONTRIBUTION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel contribution data shared with callers of this public contract.
 */
typedef struct UmiPanelContribution {
    uint32_t structure_size;
    char contribution_id[UMI_PANEL_TEXT_CAPACITY];
    char application_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char menu_path[UMI_PANEL_TEXT_CAPACITY];
    char command_id[UMI_PANEL_TEXT_CAPACITY];
    char icon_resource_id[UMI_PANEL_TEXT_CAPACITY];
    bool enabled;
    uint32_t priority;
    uint64_t revision;
} UmiPanelContribution;
/**
 * Represent the panel contribution store data shared with callers of this public contract.
 */
typedef struct UmiPanelContributionStore { UmiPanelContribution items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelContributionStore;
/**
 * Initialise panel contribution from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_contribution_init(UmiPanelContribution *record);
/**
 * Check that panel contribution satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_panel_contribution_validate(const UmiPanelContribution *record);
/**
 * Initialise panel contribution store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_contribution_store_init(UmiPanelContributionStore *store);
/**
 * Provide the panel contribution store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_contribution_store_put(UmiPanelContributionStore *store,const UmiPanelContribution *record);
/**
 * Remove panel contribution store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_contribution_store_remove(UmiPanelContributionStore *store,const char *identity);
/**
 * Find panel contribution store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelContribution *umi_panel_contribution_store_find(UmiPanelContributionStore *store,const char *identity);
/**
 * Provide the panel contribution store find const operation used by this module and its
 * client applications.
 */
const UmiPanelContribution *umi_panel_contribution_store_find_const(const UmiPanelContributionStore *store,const char *identity);
/**
 * Provide the panel contribution store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_contribution_store_snapshot(const UmiPanelContributionStore *store,UmiPanelContribution *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
