/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/layout_statistics.h
 *
 * PURPOSE:
 *   Calculate structural complexity, panel ownership, visibility, monitor,
 *   context-link and layout-depth evidence for browser summaries and design
 *   reviews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_LAYOUT_STATISTICS_H
#define UMICOM_WORKBENCH_DESIGNER_LAYOUT_STATISTICS_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_APPLICATION_STATISTICS 32U

/**
 * Represent the workbench designer application statistic data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerApplicationStatistic {
    char application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    size_t node_count;
    size_t panel_count;
    size_t hidden_count;
} UmiWorkbenchDesignerApplicationStatistic;

/**
 * Represent the workbench designer layout statistics data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerLayoutStatistics {
    size_t total_nodes;
    size_t containers;
    size_t panels;
    size_t editor_groups;
    size_t split_nodes;
    size_t tab_groups;
    size_t floating_windows;
    size_t hidden_nodes;
    size_t locked_nodes;
    size_t context_linked_nodes;
    size_t monitor_count;
    size_t maximum_depth;
    size_t maximum_children;
    double average_children;
    double complexity_score;
    UmiWorkbenchDesignerApplicationStatistic applications[UMI_WORKBENCH_DESIGNER_MAX_APPLICATION_STATISTICS];
    size_t application_count;
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerLayoutStatistics;

/**
 * Initialise workbench designer layout statistics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_layout_statistics_init(UmiWorkbenchDesignerLayoutStatistics *statistics);
/**
 * Provide the workbench designer layout statistics build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_layout_statistics_build(UmiWorkbenchDesignerLayoutStatistics *statistics, const UmiWorkbenchLayoutDocument *document);
/**
 * Provide the workbench designer layout statistics application operation used by this
 * module and its client applications.
 */
const UmiWorkbenchDesignerApplicationStatistic *umi_workbench_designer_layout_statistics_application(const UmiWorkbenchDesignerLayoutStatistics *statistics, const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
