/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_message.h
 *
 * PURPOSE:
 *   Describe one versioned extension-host protocol message.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_MESSAGE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_MESSAGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostMessage { uint32_t protocol_version; uint32_t type; uint64_t sequence; char session_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; char payload[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY]; uint64_t fingerprint; } UmiPluginExtensionHostHostMessage;
void umi_plugin_extension_host_host_message_init(UmiPluginExtensionHostHostMessage *message);
UmiStatus umi_plugin_extension_host_host_message_build(UmiPluginExtensionHostHostMessage *message, uint32_t protocol_version, uint32_t type, uint64_t sequence, const char *session_id, const char *payload);
int umi_plugin_extension_host_host_message_valid(const UmiPluginExtensionHostHostMessage *message);

#ifdef __cplusplus
}
#endif

#endif
