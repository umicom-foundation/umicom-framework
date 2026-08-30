/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/manifest_evidence.h
 *
 * PURPOSE:
 *   Capture immutable evidence derived from a parsed extension manifest.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_EVIDENCE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_EVIDENCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostManifestEvidence {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostManifestEvidence;

void umi_plugin_extension_host_manifest_evidence_init(UmiPluginExtensionHostManifestEvidence *value);
UmiStatus umi_plugin_extension_host_manifest_evidence_configure(UmiPluginExtensionHostManifestEvidence *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_manifest_evidence_validate(const UmiPluginExtensionHostManifestEvidence *value);
uint64_t umi_plugin_extension_host_manifest_evidence_fingerprint(const UmiPluginExtensionHostManifestEvidence *value);

#ifdef __cplusplus
}
#endif

#endif
