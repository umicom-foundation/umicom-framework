/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/contribution_ordering.h
 *
 * PURPOSE:
 *   Produce deterministic contribution ordering using priority and identity.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_ORDERING_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_ORDERING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host contribution ordering item data shared with callers
 * of this public contract.
 */
typedef struct UmiPluginExtensionHostContributionOrderingItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t priority; uint64_t sequence; } UmiPluginExtensionHostContributionOrderingItem;
/**
 * Provide the plugin extension host contribution ordering sort operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_contribution_ordering_sort(UmiPluginExtensionHostContributionOrderingItem *items, size_t count);
/**
 * Provide the plugin extension host contribution ordering before operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_contribution_ordering_before(const UmiPluginExtensionHostContributionOrderingItem *left, const UmiPluginExtensionHostContributionOrderingItem *right);

#ifdef __cplusplus
}
#endif

#endif
