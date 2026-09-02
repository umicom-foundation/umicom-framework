/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/manifest_validation.h
 *
 * PURPOSE:
 *   Validate extension manifest evidence before installation or activation.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_VALIDATION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_MANIFEST_VALIDATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host manifest validation data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostManifestValidation { int id_valid; int version_valid; int entry_valid; int permissions_valid; int checksum_present; int signature_present; uint32_t issue_count; } UmiPluginExtensionHostManifestValidation;
/**
 * Initialise plugin extension host manifest validation from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_manifest_validation_init(UmiPluginExtensionHostManifestValidation *result);
/**
 * Provide the plugin extension host manifest validation record operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_manifest_validation_record(UmiPluginExtensionHostManifestValidation *result, int id_valid, int version_valid, int entry_valid, int permissions_valid, int checksum_present, int signature_present);
/**
 * Provide the plugin extension host manifest validation ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_manifest_validation_ready(const UmiPluginExtensionHostManifestValidation *result);

#ifdef __cplusplus
}
#endif

#endif
