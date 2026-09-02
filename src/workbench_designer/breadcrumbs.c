/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/breadcrumbs.c
 *
 * PURPOSE:
 *   Walk parent links and reverse the resulting path into root-to-selection
 *   navigation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/breadcrumbs.h"
#include "internal.h"


/*
 * Initialise workbench designer breadcrumbs from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_breadcrumbs_init(
    UmiWorkbenchDesignerBreadcrumbs *breadcrumbs)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (breadcrumbs != NULL) (void)memset(breadcrumbs, 0, sizeof(*breadcrumbs));
}

/*
 * Provide the workbench designer breadcrumbs build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_breadcrumbs_build(
    UmiWorkbenchDesignerBreadcrumbs *breadcrumbs,
    const UmiWorkbenchLayoutDocument *document,
    const char *active_node_id)
{
    size_t path[UMI_WORKBENCH_DESIGNER_MAX_BREADCRUMBS];
    size_t path_count = 0U;
    size_t current;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (breadcrumbs == NULL || document == NULL || active_node_id == NULL ||
        active_node_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    current = umi_workbench_layout_document_find_node_index(
        document, active_node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (current == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (current != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (path_count >= UMI_WORKBENCH_DESIGNER_MAX_BREADCRUMBS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        path[path_count++] = current;
        current = umi_workbench_designer_parent_index(document, current);
    }
    umi_workbench_designer_breadcrumbs_init(breadcrumbs);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < path_count; ++index) {
        const size_t node_index = path[path_count - index - 1U];
        const UmiWorkbenchLayoutNode *node = &document->nodes[node_index];
        UmiWorkbenchDesignerBreadcrumb *item = &breadcrumbs->items[index];
        (void)umi_workbench_designer_copy_text(
            item->node_id, sizeof(item->node_id), node->node_id);
        (void)umi_workbench_designer_copy_text(
            item->label, sizeof(item->label),
            node->title[0] != '\0' ? node->title : node->node_id);
        item->kind = node->kind;
        item->node_index = node_index;
        item->active = index + 1U == path_count;
        breadcrumbs->count += 1U;
    }
    breadcrumbs->document_revision = document->version.revision;
    breadcrumbs->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer breadcrumb while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerBreadcrumb *umi_workbench_designer_breadcrumb_at(
    const UmiWorkbenchDesignerBreadcrumbs *breadcrumbs,
    size_t index)
{
    return breadcrumbs != NULL && index < breadcrumbs->count
        ? &breadcrumbs->items[index] : NULL;
}
