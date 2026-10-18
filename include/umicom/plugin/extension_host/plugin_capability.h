/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/plugin_capability.h
 *
 * PURPOSE:
 *   Describe one capability requested or exported by an extension.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PLUGIN_CAPABILITY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PLUGIN_CAPABILITY_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host plugin capability data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPluginCapability {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostPluginCapability;

/**
 * Initialise plugin extension host plugin capability from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_plugin_capability_init(UmiPluginExtensionHostPluginCapability *value);
/**
 * Provide the plugin extension host plugin capability configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_plugin_capability_configure(UmiPluginExtensionHostPluginCapability *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host plugin capability satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_plugin_capability_validate(const UmiPluginExtensionHostPluginCapability *value);
/**
 * Provide the plugin extension host plugin capability fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_plugin_capability_fingerprint(const UmiPluginExtensionHostPluginCapability *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_plugin_extension_host_plugin_capability_archive_encode(const UmiPluginExtensionHostPluginCapability *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_plugin_extension_host_plugin_capability_archive_decode(const void *bytes, size_t byte_count,
    UmiPluginExtensionHostPluginCapability *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPluginExtensionHostPluginCapability object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_plugin_extension_host_plugin_capability_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPluginExtensionHostPluginCapability *workspace, UmiPluginExtensionHostPluginCapability *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_plugin_extension_host_plugin_capability_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPluginExtensionHostPluginCapability *expected, UmiPluginExtensionHostPluginCapability *value);


#ifdef __cplusplus
}
#endif

#endif
