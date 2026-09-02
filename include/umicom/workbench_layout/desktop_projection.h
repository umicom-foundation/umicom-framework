/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/desktop_projection.h
 *
 * PURPOSE:
 *   Translate persistent semantic layout documents into the existing flat desktop window projection without making persistence depend on GTK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DESKTOP_PROJECTION_H
#define UMICOM_WORKBENCH_LAYOUT_DESKTOP_PROJECTION_H

#include "umicom/desktop/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout projection options data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutProjectionOptions {
    uint32_t structure_size;
    char default_monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutRect default_bounds;
    bool include_hidden_nodes;
    bool include_container_windows;
    bool preserve_z_order;
} UmiWorkbenchLayoutProjectionOptions;

/**
 * Represent the workbench layout projection report data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutProjectionReport {
    uint32_t structure_size;
    size_t inspected_node_count;
    size_t projected_window_count;
    size_t skipped_hidden_count;
    size_t skipped_container_count;
    size_t truncated_count;
    bool complete;
} UmiWorkbenchLayoutProjectionReport;

/**
 * Provide the workbench layout projection options default operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutProjectionOptions
umi_workbench_layout_projection_options_default(void);

/**
 * Provide the workbench layout project desktop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_project_desktop(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutProjectionOptions *options,
    UmiDesktopLayout *out_layout,
    UmiWorkbenchLayoutProjectionReport *out_report);

/**
 * Provide the workbench layout import desktop operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_import_desktop(
    const UmiDesktopLayout *desktop_layout,
    const UmiWorkbenchLayoutIdentity *identity,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutProjectionReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
