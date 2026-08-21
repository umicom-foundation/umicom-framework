/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/outline_filter.c
 *
 * PURPOSE:
 *   Apply predictable outline filtering without mutating the source tree or
 *   coupling filtering rules to a toolkit list model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/outline_filter.h"
#include "internal.h"


UmiWorkbenchDesignerOutlineQuery umi_workbench_designer_outline_query_default(void)
{
    UmiWorkbenchDesignerOutlineQuery query;
    (void)memset(&query, 0, sizeof(query));
    query.include_containers = true;
    return query;
}

static bool outline_kind_enabled(
    UmiWorkbenchLayoutNodeKind kind,
    uint32_t mask)
{
    uint32_t bit;
    if (mask == 0U) return true;
    if (kind < UMI_WORKBENCH_LAYOUT_NODE_EMPTY ||
        kind > UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) return false;
    bit = 1U << ((uint32_t)kind - 1U);
    return (mask & bit) != 0U;
}

bool umi_workbench_designer_outline_row_matches(
    const UmiWorkbenchDesignerTreeRow *row,
    const UmiWorkbenchDesignerOutlineQuery *query)
{
    bool text_match;
    if (row == NULL || query == NULL) return false;
    if (query->visible_only && !row->visible) return false;
    if (query->selected_only && !row->selected) return false;
    if (!query->include_containers && row->child_count > 0U) return false;
    if (!outline_kind_enabled(row->kind, query->node_kind_mask)) return false;
    if (query->owner_application_id[0] != '\0' &&
        !umi_workbench_designer_text_contains_case_insensitive(
            row->component_id, query->owner_application_id)) return false;
    if (query->text[0] == '\0') return true;
    text_match = umi_workbench_designer_text_contains_case_insensitive(
        row->title, query->text);
    if (!text_match) {
        text_match = umi_workbench_designer_text_contains_case_insensitive(
            row->node_id, query->text);
    }
    if (!text_match) {
        text_match = umi_workbench_designer_text_contains_case_insensitive(
            row->component_id, query->text);
    }
    return text_match;
}

UmiStatus umi_workbench_designer_outline_filter(
    const UmiWorkbenchDesignerTree *tree,
    const UmiWorkbenchDesignerOutlineQuery *query,
    UmiWorkbenchDesignerOutlineResult *out_result)
{
    size_t index;
    if (tree == NULL || query == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        if (!umi_workbench_designer_outline_row_matches(row, query)) continue;
        out_result->total_matches += 1U;
        if (out_result->count < UMI_WORKBENCH_DESIGNER_MAX_OUTLINE_RESULTS) {
            out_result->rows[out_result->count++] = *row;
        }
    }
    out_result->source_revision = tree->revision;
    out_result->revision += 1U;
    return out_result->total_matches > out_result->count
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_outline_result_at(
    const UmiWorkbenchDesignerOutlineResult *result,
    size_t index)
{
    return result != NULL && index < result->count ? &result->rows[index] : NULL;
}
