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

typedef struct UmiWorkbenchDesignerOutlineQuery {
    char text[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint32_t node_kind_mask;
    bool visible_only;
    bool selected_only;
    bool include_containers;
} UmiWorkbenchDesignerOutlineQuery;

typedef struct UmiWorkbenchDesignerOutlineResult {
    UmiWorkbenchDesignerTreeRow rows[UMI_WORKBENCH_DESIGNER_MAX_OUTLINE_RESULTS];
    size_t count;
    size_t total_matches;
    uint64_t source_revision;
    uint64_t revision;
} UmiWorkbenchDesignerOutlineResult;

UmiWorkbenchDesignerOutlineQuery umi_workbench_designer_outline_query_default(void);
bool umi_workbench_designer_outline_row_matches(const UmiWorkbenchDesignerTreeRow *row, const UmiWorkbenchDesignerOutlineQuery *query);
UmiStatus umi_workbench_designer_outline_filter(const UmiWorkbenchDesignerTree *tree, const UmiWorkbenchDesignerOutlineQuery *query, UmiWorkbenchDesignerOutlineResult *out_result);
const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_outline_result_at(const UmiWorkbenchDesignerOutlineResult *result, size_t index);

#ifdef __cplusplus
}
#endif

#endif
