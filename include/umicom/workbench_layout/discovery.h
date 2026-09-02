/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/discovery.h
 *
 * PURPOSE:
 *   Discover portable layout files at runtime, validate their semantic model
 *   and publish them through the shared template and reflection catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WORKBENCH_LAYOUT_DISCOVERY_H
#define UMICOM_WORKBENCH_LAYOUT_DISCOVERY_H

#include "umicom/reflection/engine.h"
#include "umicom/workbench_layout/template_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define bounded directory traversal and file-size policy. */
typedef struct UmiWorkbenchLayoutDiscoveryConfig {
    uint32_t structure_size;
    int recursive;
    size_t maximum_file_bytes;
    char default_module_id[UMI_REFLECTION_ID_CAPACITY];
} UmiWorkbenchLayoutDiscoveryConfig;

/** Summarise accepted, refreshed and rejected layout files. */
typedef struct UmiWorkbenchLayoutDiscoveryReport {
    uint32_t structure_size;
    size_t files_examined;
    size_t layouts_loaded;
    size_t layouts_refreshed;
    size_t invalid_layouts;
} UmiWorkbenchLayoutDiscoveryReport;

/** Return safe discovery defaults for local application layout directories. */
UmiWorkbenchLayoutDiscoveryConfig
umi_workbench_layout_discovery_config_default(void);

/**
 * Load or refresh one .umilayout file. The existing valid template is restored
 * if the replacement cannot be registered completely.
 */
UmiStatus umi_workbench_layout_discovery_load_file(
    UmiWorkbenchLayoutTemplateRegistry *templates,
    UmiReflectionEngine *reflection,
    const char *path,
    const UmiWorkbenchLayoutDiscoveryConfig *config,
    int *out_replaced);

/**
 * Walk a directory deterministically and accept valid .umilayout resources.
 * Calling this again after a file-watcher notification refreshes changed files
 * without rebuilding or restarting the host application.
 */
UmiStatus umi_workbench_layout_discover(
    UmiWorkbenchLayoutTemplateRegistry *templates,
    UmiReflectionEngine *reflection,
    const char *root,
    const UmiWorkbenchLayoutDiscoveryConfig *config,
    UmiWorkbenchLayoutDiscoveryReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
