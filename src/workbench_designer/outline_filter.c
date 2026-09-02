/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/outline_filter.c
 *
 * PURPOSE:
 *   Apply predictable outline filtering without mutating the source tree or
 *   coupling filtering rules to a toolkit list model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/outline_filter.h"
#include "internal.h"


/*
 * Provide the workbench designer outline query default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerOutlineQuery umi_workbench_designer_outline_query_default(void)
{
    UmiWorkbenchDesignerOutlineQuery query;
    (void)memset(&query, 0, sizeof(query));
    query.include_containers = true;
    return query;
}

/*
 * Provide the outline kind enabled operation used by this module and its client
 * applications.
 */
static bool outline_kind_enabled(
    UmiWorkbenchLayoutNodeKind kind,
    uint32_t mask)
{
    uint32_t bit;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mask == 0U) return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind < UMI_WORKBENCH_LAYOUT_NODE_EMPTY ||
        kind > UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) return false;
    bit = 1U << ((uint32_t)kind - 1U);
    return (mask & bit) != 0U;
}

/*
 * Provide the workbench designer outline row matches operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_outline_row_matches(
    const UmiWorkbenchDesignerTreeRow *row,
    const UmiWorkbenchDesignerOutlineQuery *query)
{
    bool text_match;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL || query == NULL) return false;
    /* Apply this operation only while the related capability or state is available. */
    if (query->visible_only && !row->visible) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->selected_only && !row->selected) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!query->include_containers && row->child_count > 0U) return false;
    /* Apply this operation only while the related capability or state is available. */
    if (!outline_kind_enabled(row->kind, query->node_kind_mask)) return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (query->owner_application_id[0] != '\0' &&
        !umi_workbench_designer_text_contains_case_insensitive(
            row->component_id, query->owner_application_id)) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->text[0] == '\0') return true;
    text_match = umi_workbench_designer_text_contains_case_insensitive(
        row->title, query->text);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!text_match) {
        text_match = umi_workbench_designer_text_contains_case_insensitive(
            row->node_id, query->text);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!text_match) {
        text_match = umi_workbench_designer_text_contains_case_insensitive(
            row->component_id, query->text);
    }
    return text_match;
}

/*
 * Provide the workbench designer outline filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_outline_filter(
    const UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchDesignerOutlineQuery *query,
    UmiWorkbenchDesignerOutlineResult *out_result)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || query == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_designer_outline_row_matches(row, query)) continue;
        out_result->total_matches += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_result->count < UMI_WORKBENCH_DESIGNER_MAX_OUTLINE_RESULTS) {
            out_result->rows[out_result->count++] = *row;
        }
    }
    out_result->source_revision = tree->revision;
    out_result->revision += 1U;
    return out_result->total_matches > out_result->count
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/*
 * Find workbench designer outline result while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_outline_result_at(
    const UmiWorkbenchDesignerOutlineResult *result,
    size_t index)
{
    return result != NULL && index < result->count ? &result->rows[index] : NULL;
}
