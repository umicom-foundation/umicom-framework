/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/trust_level.h
 *
 * PURPOSE:
 *   Calculate extension trust from signature, publisher and provenance evidence.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_TRUST_LEVEL_H
#define UMICOM_PLUGIN_EXTENSION_HOST_TRUST_LEVEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostTrustLevelEvidence { int signature_valid; int publisher_known; int checksum_valid; int provenance_complete; uint32_t risk; } UmiPluginExtensionHostTrustLevelEvidence;
uint32_t umi_plugin_extension_host_trust_level_score(const UmiPluginExtensionHostTrustLevelEvidence *evidence);
UmiPluginExtensionHostTrust umi_plugin_extension_host_trust_level_classify(const UmiPluginExtensionHostTrustLevelEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
