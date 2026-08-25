/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/dependency_resolution.h
 *
 * PURPOSE:
 *   Evaluate whether required extension dependencies are present and compatible.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_RESOLUTION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_RESOLUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostDependencyResolution { uint32_t required; uint32_t satisfied; uint32_t incompatible; uint32_t optional_missing; } UmiPluginExtensionHostDependencyResolution;
void umi_plugin_extension_host_dependency_resolution_init(UmiPluginExtensionHostDependencyResolution *result);
void umi_plugin_extension_host_dependency_resolution_record(UmiPluginExtensionHostDependencyResolution *result, int required, int present, int compatible);
int umi_plugin_extension_host_dependency_resolution_ready(const UmiPluginExtensionHostDependencyResolution *result);
uint32_t umi_plugin_extension_host_dependency_resolution_missing_required(const UmiPluginExtensionHostDependencyResolution *result);

#ifdef __cplusplus
}
#endif

#endif
