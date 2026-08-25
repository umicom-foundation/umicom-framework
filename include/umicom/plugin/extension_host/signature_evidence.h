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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_SIGNATURE_EVIDENCE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_SIGNATURE_EVIDENCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_plugin_extension_host_signature_evidence_init(UmiPluginExtensionHostSignatureEvidence *value);
UmiStatus umi_plugin_extension_host_signature_evidence_configure(UmiPluginExtensionHostSignatureEvidence *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_signature_evidence_validate(const UmiPluginExtensionHostSignatureEvidence *value);
uint64_t umi_plugin_extension_host_signature_evidence_fingerprint(const UmiPluginExtensionHostSignatureEvidence *value);

#ifdef __cplusplus
}
#endif

#endif
