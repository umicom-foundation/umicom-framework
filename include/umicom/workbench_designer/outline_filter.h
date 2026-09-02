/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/outline_filter.h
 *
 * PURPOSE:
 *   Filter the semantic layout outline by text, application, node kind and
 *   visibility while retaining stable row identities for frontend selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_OUTLINE_FILTER_H
#define UMICOM_WORKBENCH_DESIGNER_OUTLINE_FILTER_H

#include "umicom/workbench_designer/tree.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_OUTLINE_RESULTS 256U

/**
 * Represent the workbench designer outline query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerOutlineQuery {
    char text[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint32_t node_kind_mask;
    bool visible_only;
    bool selected_only;
    bool include_containers;
} UmiWorkbenchDesignerOutlineQuery;

/**
 * Represent the workbench designer outline result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerOutlineResult {
    UmiWorkbenchDesignerTreeRow rows[UMI_WORKBENCH_DESIGNER_MAX_OUTLINE_RESULTS];
    size_t count;
    size_t total_matches;
    uint64_t source_revision;
    uint64_t revision;
} UmiWorkbenchDesignerOutlineResult;

/**
 * Provide the workbench designer outline query default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerOutlineQuery umi_workbench_designer_outline_query_default(void);
/**
 * Provide the workbench designer outline row matches operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_outline_row_matches(const UmiWorkbenchDesignerTreeRow *row, const UmiWorkbenchDesignerOutlineQuery *query);
/**
 * Provide the workbench designer outline filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_outline_filter(const UmiWorkbenchDesignerTree *tree, const UmiWorkbenchDesignerOutlineQuery *query, UmiWorkbenchDesignerOutlineResult *out_result);
/**
 * Find workbench designer outline result while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_outline_result_at(const UmiWorkbenchDesignerOutlineResult *result, size_t index);

#ifdef __cplusplus
}
#endif

#endif
