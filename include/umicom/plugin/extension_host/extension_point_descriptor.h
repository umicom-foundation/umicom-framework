/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/extension_point_descriptor.h
 *
 * PURPOSE:
 *   Describe one Framework extension point and its required contract version.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_POINT_DESCRIPTOR_H
#define UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_POINT_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostExtensionPointDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostExtensionPointDescriptor;

void umi_plugin_extension_host_extension_point_descriptor_init(UmiPluginExtensionHostExtensionPointDescriptor *value);
UmiStatus umi_plugin_extension_host_extension_point_descriptor_configure(UmiPluginExtensionHostExtensionPointDescriptor *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_extension_point_descriptor_validate(const UmiPluginExtensionHostExtensionPointDescriptor *value);
uint64_t umi_plugin_extension_host_extension_point_descriptor_fingerprint(const UmiPluginExtensionHostExtensionPointDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
