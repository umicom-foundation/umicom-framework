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
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host host message data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginExtensionHostHostMessage { uint32_t protocol_version; uint32_t type; uint64_t sequence; char session_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; char payload[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY]; uint64_t fingerprint; } UmiPluginExtensionHostHostMessage;
/**
 * Initialise plugin extension host host message from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_message_init(UmiPluginExtensionHostHostMessage *message);
/**
 * Provide the plugin extension host host message build operation used by this module and
 * its client applications.
 */
UmiStatus umi_plugin_extension_host_host_message_build(UmiPluginExtensionHostHostMessage *message, uint32_t protocol_version, uint32_t type, uint64_t sequence, const char *session_id, const char *payload);
/**
 * Check that plugin extension host host message satisfies its contract before another
 * service relies on it.
 */
int umi_plugin_extension_host_host_message_valid(const UmiPluginExtensionHostHostMessage *message);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_plugin_extension_host_host_message_archive_encode(const UmiPluginExtensionHostHostMessage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_plugin_extension_host_host_message_archive_decode(const void *bytes, size_t byte_count,
    UmiPluginExtensionHostHostMessage *value);

#ifdef __cplusplus
}
#endif

#endif
