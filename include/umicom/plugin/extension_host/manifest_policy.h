/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/manifest_policy.h
 *
 * PURPOSE:
 *   Define host policy for extension manifests, signatures and permission declarations.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostManifestPolicy { int require_signature; int require_checksum; int allow_unknown_fields; uint32_t maximum_permissions; uint32_t maximum_risk; } UmiPluginExtensionHostManifestPolicy;
void umi_plugin_extension_host_manifest_policy_init(UmiPluginExtensionHostManifestPolicy *policy);
UmiPluginExtensionHostDecision umi_plugin_extension_host_manifest_policy_evaluate(const UmiPluginExtensionHostManifestPolicy *policy, int signed_package, int checksum_verified, uint32_t permission_count, uint32_t risk);

#ifdef __cplusplus
}
#endif

#endif
