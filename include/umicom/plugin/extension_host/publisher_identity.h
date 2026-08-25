/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/publisher_identity.h
 *
 * PURPOSE:
 *   Describe an extension publisher identity independently of a package version.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PUBLISHER_IDENTITY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PUBLISHER_IDENTITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPublisherIdentity {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostPublisherIdentity;

void umi_plugin_extension_host_publisher_identity_init(UmiPluginExtensionHostPublisherIdentity *value);
UmiStatus umi_plugin_extension_host_publisher_identity_configure(UmiPluginExtensionHostPublisherIdentity *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_publisher_identity_validate(const UmiPluginExtensionHostPublisherIdentity *value);
uint64_t umi_plugin_extension_host_publisher_identity_fingerprint(const UmiPluginExtensionHostPublisherIdentity *value);

#ifdef __cplusplus
}
#endif

#endif
