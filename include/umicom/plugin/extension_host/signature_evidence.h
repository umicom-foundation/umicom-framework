/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/signature_evidence.h
 *
 * PURPOSE:
 *   Capture signature-verification evidence supplied by the Framework security layer.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_SIGNATURE_EVIDENCE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_SIGNATURE_EVIDENCE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host signature evidence data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostSignatureEvidence {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostSignatureEvidence;

/**
 * Initialise plugin extension host signature evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_signature_evidence_init(UmiPluginExtensionHostSignatureEvidence *value);
/**
 * Provide the plugin extension host signature evidence configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_signature_evidence_configure(UmiPluginExtensionHostSignatureEvidence *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host signature evidence satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_plugin_extension_host_signature_evidence_validate(const UmiPluginExtensionHostSignatureEvidence *value);
/**
 * Provide the plugin extension host signature evidence fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_signature_evidence_fingerprint(const UmiPluginExtensionHostSignatureEvidence *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_plugin_extension_host_signature_evidence_archive_encode(const UmiPluginExtensionHostSignatureEvidence *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_plugin_extension_host_signature_evidence_archive_decode(const void *bytes, size_t byte_count,
    UmiPluginExtensionHostSignatureEvidence *value);

#ifdef __cplusplus
}
#endif

#endif
