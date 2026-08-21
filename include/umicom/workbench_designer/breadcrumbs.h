/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/breadcrumbs.h
 *
 * PURPOSE:
 *   Build a semantic breadcrumb path from the layout root to the active node for
 *   navigation and context inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BREADCRUMBS_H
#define UMICOM_WORKBENCH_DESIGNER_BREADCRUMBS_H

#include "umicom/workbench_layout/document.h"
#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_BREADCRUMBS 32U

typedef struct UmiWorkbenchDesignerBreadcrumb {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    size_t node_index;
    bool active;
} UmiWorkbenchDesignerBreadcrumb;

typedef struct UmiWorkbenchDesignerBreadcrumbs {
    UmiWorkbenchDesignerBreadcrumb items[UMI_WORKBENCH_DESIGNER_MAX_BREADCRUMBS];
    size_t count;
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerBreadcrumbs;

void umi_workbench_designer_breadcrumbs_init(
    UmiWorkbenchDesignerBreadcrumbs *breadcrumbs);
UmiStatus umi_workbench_designer_breadcrumbs_build(
    UmiWorkbenchDesignerBreadcrumbs *breadcrumbs,
    const UmiWorkbenchLayoutDocument *document,
    const char *active_node_id);
const UmiWorkbenchDesignerBreadcrumb *umi_workbench_designer_breadcrumb_at(
    const UmiWorkbenchDesignerBreadcrumbs *breadcrumbs,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
