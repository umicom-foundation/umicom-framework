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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host update channel data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise plugin extension host update channel from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_update_channel_init(UmiPluginExtensionHostUpdateChannel *value);
/**
 * Provide the plugin extension host update channel configure operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_update_channel_configure(UmiPluginExtensionHostUpdateChannel *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host update channel satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_update_channel_validate(const UmiPluginExtensionHostUpdateChannel *value);
/**
 * Provide the plugin extension host update channel fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_update_channel_fingerprint(const UmiPluginExtensionHostUpdateChannel *value);

#ifdef __cplusplus
}
#endif

#endif
