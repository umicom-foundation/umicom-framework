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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host dependency resolution data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostDependencyResolution { uint32_t required; uint32_t satisfied; uint32_t incompatible; uint32_t optional_missing; } UmiPluginExtensionHostDependencyResolution;
/**
 * Initialise plugin extension host dependency resolution from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_dependency_resolution_init(UmiPluginExtensionHostDependencyResolution *result);
/**
 * Provide the plugin extension host dependency resolution record operation used by this
 * module and its client applications.
 */
void umi_plugin_extension_host_dependency_resolution_record(UmiPluginExtensionHostDependencyResolution *result, int required, int present, int compatible);
/**
 * Provide the plugin extension host dependency resolution ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_resolution_ready(const UmiPluginExtensionHostDependencyResolution *result);
/**
 * Provide the plugin extension host dependency resolution missing required operation used
 * by this module and its client applications.
 */
uint32_t umi_plugin_extension_host_dependency_resolution_missing_required(const UmiPluginExtensionHostDependencyResolution *result);

#ifdef __cplusplus
}
#endif

#endif
