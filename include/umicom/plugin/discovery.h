/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/discovery.h
 *
 * PURPOSE:
 *   Discover manifest files deterministically without loading executable plug-in
 *   code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_DISCOVERY_H
#define UMICOM_PLUGIN_DISCOVERY_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiPluginDiscoverySink)(const char *manifest_path,
                                            const UmiPluginManifest *manifest,
                                            void *user_data);
/**
 * Represent the plugin discovery report data shared with callers of this public contract.
 */
typedef struct UmiPluginDiscoveryReport { size_t files_examined; size_t manifests_loaded; size_t invalid_manifests; } UmiPluginDiscoveryReport;
/**
 * Provide the plugin discover operation used by this module and its client applications.
 */
UmiStatus umi_plugin_discover(const char *root,
                              int recursive,
                              UmiPluginDiscoverySink sink,
                              void *user_data,
                              UmiPluginDiscoveryReport *out_report);
#ifdef __cplusplus
}
#endif

#endif
