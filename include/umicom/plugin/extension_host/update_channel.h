/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/update_channel.h
 *
 * PURPOSE:
 *   Describe a trusted extension update channel and its policy metadata.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_UPDATE_CHANNEL_H
#define UMICOM_PLUGIN_EXTENSION_HOST_UPDATE_CHANNEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostUpdateChannel {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostUpdateChannel;

void umi_plugin_extension_host_update_channel_init(UmiPluginExtensionHostUpdateChannel *value);
UmiStatus umi_plugin_extension_host_update_channel_configure(UmiPluginExtensionHostUpdateChannel *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_update_channel_validate(const UmiPluginExtensionHostUpdateChannel *value);
uint64_t umi_plugin_extension_host_update_channel_fingerprint(const UmiPluginExtensionHostUpdateChannel *value);

#ifdef __cplusplus
}
#endif

#endif
