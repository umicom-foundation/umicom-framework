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
/**
 * Represent the plugin contribution data shared with callers of this public contract.
 */
typedef struct UmiPluginContribution {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char contribution_type[UMI_PLUGIN_CONTRIBUTION_TYPE_CAPACITY];
    char contribution_id[UMI_PLUGIN_CONTRIBUTION_ID_CAPACITY];
    char target[UMI_PLUGIN_CONTRIBUTION_TARGET_CAPACITY];
    int32_t order;
} UmiPluginContribution;
/**
 * Represent the plugin contribution registry data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginContributionRegistry UmiPluginContributionRegistry;
/**
 * Initialise plugin contribution registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_contribution_registry_create(UmiPluginContributionRegistry **out_registry);
/**
 * Release or reset state held by plugin contribution registry so the same storage can be
 * reused safely.
 */
void umi_plugin_contribution_registry_destroy(UmiPluginContributionRegistry *registry);
/**
 * Add plugin contribution registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_plugin_contribution_registry_add(UmiPluginContributionRegistry *registry, const UmiPluginContribution *contribution);
/**
 * Return the number of records represented by plugin contribution registry without
 * changing their state.
 */
size_t umi_plugin_contribution_registry_count(const UmiPluginContributionRegistry *registry);
/**
 * Find plugin contribution registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_plugin_contribution_registry_at(const UmiPluginContributionRegistry *registry, size_t index, UmiPluginContribution *out_contribution);
/**
 * Provide the plugin contribution registry count type operation used by this module and
 * its client applications.
 */
size_t umi_plugin_contribution_registry_count_type(const UmiPluginContributionRegistry *registry, const char *type);
#ifdef __cplusplus
}
#endif

#endif
