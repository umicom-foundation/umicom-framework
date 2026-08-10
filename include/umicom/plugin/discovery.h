/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/discovery.h
 *
 * PURPOSE:
 *   Discover manifest files deterministically without loading executable plug-in
 *   code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiPluginDiscoveryReport { size_t files_examined; size_t manifests_loaded; size_t invalid_manifests; } UmiPluginDiscoveryReport;
UmiStatus umi_plugin_discover(const char *root,
                              int recursive,
                              UmiPluginDiscoverySink sink,
                              void *user_data,
                              UmiPluginDiscoveryReport *out_report);
#ifdef __cplusplus
}
#endif

#endif
