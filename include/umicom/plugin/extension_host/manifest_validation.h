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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostManifestValidation { int id_valid; int version_valid; int entry_valid; int permissions_valid; int checksum_present; int signature_present; uint32_t issue_count; } UmiPluginExtensionHostManifestValidation;
void umi_plugin_extension_host_manifest_validation_init(UmiPluginExtensionHostManifestValidation *result);
UmiStatus umi_plugin_extension_host_manifest_validation_record(UmiPluginExtensionHostManifestValidation *result, int id_valid, int version_valid, int entry_valid, int permissions_valid, int checksum_present, int signature_present);
int umi_plugin_extension_host_manifest_validation_ready(const UmiPluginExtensionHostManifestValidation *result);

#ifdef __cplusplus
}
#endif

#endif
