/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/checksum_policy.h
 *
 * PURPOSE:
 *   Apply checksum evidence requirements before package trust or installation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CHECKSUM_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CHECKSUM_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostChecksumPolicy { int require_digest; int require_match; uint32_t minimum_hex_length; } UmiPluginExtensionHostChecksumPolicy;
void umi_plugin_extension_host_checksum_policy_init(UmiPluginExtensionHostChecksumPolicy *policy);
UmiPluginExtensionHostDecision umi_plugin_extension_host_checksum_policy_evaluate(const UmiPluginExtensionHostChecksumPolicy *policy, const char *digest, int digest_matches);

#ifdef __cplusplus
}
#endif

#endif
