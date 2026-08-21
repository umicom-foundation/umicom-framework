/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/layout_statistics.c
 *
 * PURPOSE:
 *   Traverse semantic layout ownership and topology to calculate bounded design-
 *   review statistics without rendering widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/layout_statistics.h"
#include "internal.h"


void umi_workbench_designer_layout_statistics_init(
    UmiWorkbenchDesignerLayoutStatistics *statistics)
{
    if (statistics == NULL) return;
    (void)memset(statistics, 0, sizeof(*statistics));
}

static size_t statistics_depth(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index)
{
    size_t depth = 0U;
    size_t current = node_index;
    size_t guard = 0U;
    while (current < document->node_count && guard < document->node_count) {
        const size_t parent = document->nodes[current].parent_index;
        if (parent == UMI_WORKBENCH_LAYOUT_INDEX_NONE) break;
        depth += 1U;
        current = parent;
        guard += 1U;
    }
    return depth;
}

static UmiStatus statistics_application(
    UmiWorkbenchDesignerLayoutStatistics *statistics,
    const UmiWorkbenchLayoutNode *node)
{
    UmiWorkbenchDesignerApplicationStatistic *item = NULL;
    size_t index;
    if (node->owner_application_id[0] == '\0') return UMI_STATUS_OK;
    for (index = 0U; index < statistics->application_count; ++index) {
        if (strcmp(statistics->applications[index].application_id,
                   node->owner_application_id) == 0) {
            item = &statistics->applications[index];
            break;
        }
    }
    if (item == NULL) {
        if (statistics->application_count >=
            UMI_WORKBENCH_DESIGNER_MAX_APPLICATION_STATISTICS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item = &statistics->applications[statistics->application_count++];
        (void)memset(item, 0, sizeof(*item));
        (void)umi_workbench_designer_copy_text(
            item->application_id, sizeof(item->application_id),
            node->owner_application_id);
    }
    item->node_count += 1U;
    if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
        node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) {
        item->panel_count += 1U;
    }
    if (node->visibility == UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN) {
        item->hidden_count += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_layout_statistics_build(
    UmiWorkbenchDesignerLayoutStatistics *statistics,
    const UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    size_t child_total = 0U;
    char monitors[UMI_WORKBENCH_DESIGNER_MAX_MONITORS]
        [UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    size_t monitor_count = 0U;
    if (statistics == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_layout_statistics_init(statistics);
    (void)memset(monitors, 0, sizeof(monitors));
    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        const size_t depth = statistics_depth(document, index);
        UmiStatus status;
        statistics->total_nodes += 1U;
        child_total += node->child_count;
        if (depth > statistics->maximum_depth) statistics->maximum_depth = depth;
        if (node->child_count > statistics->maximum_children) {
            statistics->maximum_children = node->child_count;
        }
        if (umi_workbench_layout_node_is_container(node)) statistics->containers += 1U;
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL) statistics->panels += 1U;
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) statistics->editor_groups += 1U;
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) statistics->split_nodes += 1U;
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP) statistics->tab_groups += 1U;
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) statistics->floating_windows += 1U;
        if (node->visibility == UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN) statistics->hidden_nodes += 1U;
        if ((node->flags & UMI_WORKBENCH_LAYOUT_NODE_LOCKED) != 0U) statistics->locked_nodes += 1U;
        if (node->context_group_id[0] != '\0') statistics->context_linked_nodes += 1U;
        if (node->monitor_id[0] != '\0') {
            size_t monitor;
            bool found = false;
            for (monitor = 0U; monitor < monitor_count; ++monitor) {
                if (strcmp(monitors[monitor], node->monitor_id) == 0) found = true;
            }
            if (!found && monitor_count < UMI_WORKBENCH_DESIGNER_MAX_MONITORS) {
                (void)umi_workbench_designer_copy_text(
                    monitors[monitor_count], sizeof(monitors[monitor_count]),
                    node->monitor_id);
                monitor_count += 1U;
            }
        }
        status = statistics_application(statistics, node);
        if (status != UMI_STATUS_OK) return status;
    }
    statistics->monitor_count = monitor_count;
    statistics->average_children = document->node_count > 0U
        ? (double)child_total / (double)document->node_count : 0.0;
    statistics->complexity_score =
        (double)statistics->total_nodes +
        (double)statistics->maximum_depth * 2.0 +
        (double)statistics->floating_windows * 2.5 +
        (double)statistics->context_linked_nodes * 0.5 +
        (double)statistics->application_count * 1.5;
    statistics->document_revision = document->version.revision;
    statistics->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerApplicationStatistic *umi_workbench_designer_layout_statistics_application(
    const UmiWorkbenchDesignerLayoutStatistics *statistics,
    const char *application_id)
{
    size_t index;
    if (statistics == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < statistics->application_count; ++index) {
        if (strcmp(statistics->applications[index].application_id,
                   application_id) == 0) return &statistics->applications[index];
    }
    return NULL;
}
