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
typedef struct UmiPanelContributionStore { UmiPanelContribution items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelContributionStore;
void umi_panel_contribution_init(UmiPanelContribution *record);
UmiStatus umi_panel_contribution_validate(const UmiPanelContribution *record);
void umi_panel_contribution_store_init(UmiPanelContributionStore *store);
UmiStatus umi_panel_contribution_store_put(UmiPanelContributionStore *store,const UmiPanelContribution *record);
UmiStatus umi_panel_contribution_store_remove(UmiPanelContributionStore *store,const char *identity);
UmiPanelContribution *umi_panel_contribution_store_find(UmiPanelContributionStore *store,const char *identity);
const UmiPanelContribution *umi_panel_contribution_store_find_const(const UmiPanelContributionStore *store,const char *identity);
UmiStatus umi_panel_contribution_store_snapshot(const UmiPanelContributionStore *store,UmiPanelContribution *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
