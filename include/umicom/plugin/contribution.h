/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/contribution.h
 *
 * PURPOSE:
 *   Register typed plug-in contributions for commands, panes, menus, tools,
 *   providers and product extensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_CONTRIBUTION_H
#define UMICOM_PLUGIN_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PLUGIN_CONTRIBUTION_TYPE_CAPACITY 96U
#define UMI_PLUGIN_CONTRIBUTION_ID_CAPACITY 160U
#define UMI_PLUGIN_CONTRIBUTION_TARGET_CAPACITY 160U
#define UMI_PLUGIN_CONTRIBUTION_MAX 2048U
typedef struct UmiPluginContribution {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char contribution_type[UMI_PLUGIN_CONTRIBUTION_TYPE_CAPACITY];
    char contribution_id[UMI_PLUGIN_CONTRIBUTION_ID_CAPACITY];
    char target[UMI_PLUGIN_CONTRIBUTION_TARGET_CAPACITY];
    int32_t order;
} UmiPluginContribution;
typedef struct UmiPluginContributionRegistry UmiPluginContributionRegistry;
UmiStatus umi_plugin_contribution_registry_create(UmiPluginContributionRegistry **out_registry);
void umi_plugin_contribution_registry_destroy(UmiPluginContributionRegistry *registry);
UmiStatus umi_plugin_contribution_registry_add(UmiPluginContributionRegistry *registry, const UmiPluginContribution *contribution);
size_t umi_plugin_contribution_registry_count(const UmiPluginContributionRegistry *registry);
UmiStatus umi_plugin_contribution_registry_at(const UmiPluginContributionRegistry *registry, size_t index, UmiPluginContribution *out_contribution);
size_t umi_plugin_contribution_registry_count_type(const UmiPluginContributionRegistry *registry, const char *type);
#ifdef __cplusplus
}
#endif

#endif
