/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/contribution_descriptor.h
 *
 * PURPOSE:
 *   Describe one extension contribution to a Framework extension point.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_DESCRIPTOR_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host contribution descriptor data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostContributionDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostContributionDescriptor;

/**
 * Initialise plugin extension host contribution descriptor from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_contribution_descriptor_init(UmiPluginExtensionHostContributionDescriptor *value);
/**
 * Provide the plugin extension host contribution descriptor configure operation used by
 * this module and its client applications.
 */
UmiStatus umi_plugin_extension_host_contribution_descriptor_configure(UmiPluginExtensionHostContributionDescriptor *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host contribution descriptor satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_plugin_extension_host_contribution_descriptor_validate(const UmiPluginExtensionHostContributionDescriptor *value);
/**
 * Provide the plugin extension host contribution descriptor fingerprint operation used by
 * this module and its client applications.
 */
uint64_t umi_plugin_extension_host_contribution_descriptor_fingerprint(const UmiPluginExtensionHostContributionDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
