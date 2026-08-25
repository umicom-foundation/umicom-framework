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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

void umi_plugin_extension_host_contribution_descriptor_init(UmiPluginExtensionHostContributionDescriptor *value);
UmiStatus umi_plugin_extension_host_contribution_descriptor_configure(UmiPluginExtensionHostContributionDescriptor *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_contribution_descriptor_validate(const UmiPluginExtensionHostContributionDescriptor *value);
uint64_t umi_plugin_extension_host_contribution_descriptor_fingerprint(const UmiPluginExtensionHostContributionDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
