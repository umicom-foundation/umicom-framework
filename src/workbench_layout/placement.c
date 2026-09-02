/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/placement.c
 *
 * PURPOSE:
 *   Implement portable window restoration, monitor relocation, geometry clamping and deterministic cascading after topology changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/placement.h"

#include <limits.h>
#include <string.h>

#include "internal.h"

/* Provide the clamp i32 operation used by this module and its client applications. */
static int32_t clamp_i32(
    int32_t value,
    int32_t minimum,
    int32_t maximum)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value < minimum) {
        return minimum;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value > maximum) {
        return maximum;
    }
    return value;
}

/*
 * Return the number of records represented by effective minimum without changing their
 * state.
 */
static UmiWorkbenchLayoutSize effective_minimum_size(
    const UmiWorkbenchLayoutNode *node)
{
    UmiWorkbenchLayoutSize minimum = {160, 100};

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node != NULL) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->minimum_size.width > 0) {
            minimum.width = node->minimum_size.width;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->minimum_size.height > 0) {
            minimum.height = node->minimum_size.height;
        }
    }
    return minimum;
}

/*
 * Return the number of records represented by effective window without changing their
 * state.
 */
static UmiWorkbenchLayoutSize effective_window_size(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutPlacementOptions *options)
{
    UmiWorkbenchLayoutSize size = options->default_size;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node != NULL &&
        options->preserve_size &&
        node->bounds.width > 0 &&
        node->bounds.height > 0) {
        size.width = node->bounds.width;
        size.height = node->bounds.height;
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (node != NULL &&
               node->preferred_size.width > 0 &&
               node->preferred_size.height > 0) {
        size = node->preferred_size;
    }
    return size;
}

/* Provide the rect equal operation used by this module and its client applications. */
static bool rect_equal(
    const UmiWorkbenchLayoutRect *left,
    const UmiWorkbenchLayoutRect *right)
{
    return left != NULL && right != NULL &&
           left->x == right->x &&
           left->y == right->y &&
           left->width == right->width &&
           left->height == right->height;
}

/*
 * Provide the workbench layout placement options default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutPlacementOptions
umi_workbench_layout_placement_options_default(void)
{
    UmiWorkbenchLayoutPlacementOptions options;

    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.cascade_offset_x = 28;
    options.cascade_offset_y = 28;
    options.default_size.width = 1024;
    options.default_size.height = 720;
    options.preserve_size = true;
    options.prefer_recorded_monitor = true;
    options.clamp_to_work_area = true;
    options.relocate_offscreen_windows = true;
    return options;
}

/*
 * Provide the workbench layout clamp rect operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutRect umi_workbench_layout_clamp_rect(
    const UmiWorkbenchLayoutRect *rect,
    const UmiWorkbenchLayoutRect *work_area,
    const UmiWorkbenchLayoutSize *minimum_size)
{
    UmiWorkbenchLayoutRect result = {0, 0, 0, 0};
    int32_t minimum_width = 1;
    int32_t minimum_height = 1;
    int32_t maximum_x;
    int32_t maximum_y;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rect == NULL || work_area == NULL ||
        !umi_workbench_layout_rect_is_valid(work_area)) {
        return result;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (minimum_size != NULL) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (minimum_size->width > minimum_width) {
            minimum_width = minimum_size->width;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (minimum_size->height > minimum_height) {
            minimum_height = minimum_size->height;
        }
    }

    result.width = rect->width > 0
        ? rect->width
        : minimum_width;
    result.height = rect->height > 0
        ? rect->height
        : minimum_height;

    result.width = clamp_i32(
        result.width,
        minimum_width,
        work_area->width);
    result.height = clamp_i32(
        result.height,
        minimum_height,
        work_area->height);

    maximum_x = work_area->x +
        work_area->width - result.width;
    maximum_y = work_area->y +
        work_area->height - result.height;
    result.x = clamp_i32(rect->x, work_area->x, maximum_x);
    result.y = clamp_i32(rect->y, work_area->y, maximum_y);
    return result;
}

/*
 * Provide the workbench layout cascade rect operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutRect umi_workbench_layout_cascade_rect(
    const UmiWorkbenchLayoutRect *work_area,
    const UmiWorkbenchLayoutSize *size,
    int32_t offset_x,
    int32_t offset_y,
    size_t cascade_index)
{
    UmiWorkbenchLayoutRect result = {0, 0, 0, 0};
    UmiWorkbenchLayoutSize minimum = {160, 100};
    int64_t requested_x;
    int64_t requested_y;
    int64_t cycle_width;
    int64_t cycle_height;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (work_area == NULL || size == NULL ||
        !umi_workbench_layout_rect_is_valid(work_area)) {
        return result;
    }

    result.width = size->width > 0
        ? size->width
        : 1024;
    result.height = size->height > 0
        ? size->height
        : 720;

    cycle_width = work_area->width > result.width
        ? (int64_t)work_area->width - result.width
        : 1;
    cycle_height = work_area->height > result.height
        ? (int64_t)work_area->height - result.height
        : 1;

    requested_x = (int64_t)work_area->x +
        ((int64_t)cascade_index * offset_x) % cycle_width;
    requested_y = (int64_t)work_area->y +
        ((int64_t)cascade_index * offset_y) % cycle_height;

    /* Apply this branch only when its contract condition is satisfied. */
    if (requested_x > INT32_MAX) {
        requested_x = INT32_MAX;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (requested_y > INT32_MAX) {
        requested_y = INT32_MAX;
    }
    result.x = (int32_t)requested_x;
    result.y = (int32_t)requested_y;
    return umi_workbench_layout_clamp_rect(
        &result, work_area, &minimum);
}

/*
 * Provide the workbench layout place node operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_place_node(
    UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutPlacementOptions *options,
    size_t cascade_index,
    bool *out_changed)
{
    UmiWorkbenchLayoutPlacementOptions effective;
    const UmiWorkbenchMonitor *monitor = NULL;
    UmiWorkbenchLayoutRect original;
    char original_monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutRect placed;
    UmiWorkbenchLayoutSize minimum;
    UmiWorkbenchLayoutSize size;
    bool recorded_monitor_missing = false;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || topology == NULL ||
        out_changed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_changed = false;

    /* Apply this branch only when its contract condition is satisfied. */
    if (node->kind != UMI_WORKBENCH_LAYOUT_NODE_WINDOW &&
        node->kind !=
            UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) {
        return UMI_STATUS_OK;
    }

    effective = options != NULL
        ? *options
        : umi_workbench_layout_placement_options_default();

    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.prefer_recorded_monitor &&
        node->monitor_id[0] != '\0') {
        monitor = umi_workbench_monitor_topology_find(
            topology, node->monitor_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (monitor != NULL && !monitor->enabled) {
            monitor = NULL;
        }
        recorded_monitor_missing = monitor == NULL;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL &&
        !recorded_monitor_missing &&
        umi_workbench_layout_rect_is_valid(&node->bounds)) {
        monitor = umi_workbench_monitor_topology_best_for_rect(
            topology, &node->bounds);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) {
        monitor = umi_workbench_monitor_topology_primary(topology);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    original = node->bounds;
    (void)umi_workbench_layout_copy_text(
        original_monitor_id,
        sizeof(original_monitor_id),
        node->monitor_id,
        true);
    minimum = effective_minimum_size(node);
    size = effective_window_size(node, &effective);

    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_rect_is_valid(&node->bounds) ||
        (effective.relocate_offscreen_windows &&
         !umi_workbench_layout_rect_intersects(
             &node->bounds, &monitor->work_area))) {
        placed = umi_workbench_layout_cascade_rect(
            &monitor->work_area,
            &size,
            effective.cascade_offset_x,
            effective.cascade_offset_y,
            cascade_index);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (effective.clamp_to_work_area) {
        placed = umi_workbench_layout_clamp_rect(
            &node->bounds,
            &monitor->work_area,
            &minimum);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        placed = node->bounds;
    }

    status = umi_workbench_layout_node_set_monitor(
        node, monitor->monitor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_workbench_layout_node_set_bounds(node, &placed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    *out_changed =
        !rect_equal(&original, &placed) ||
        strcmp(original_monitor_id, monitor->monitor_id) != 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout place document operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_place_document(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchLayoutPlacementOptions *options,
    UmiWorkbenchLayoutPlacementReport *out_report)
{
    UmiWorkbenchLayoutPlacementReport report;
    UmiWorkbenchLayoutPlacementOptions effective;
    size_t index;
    size_t cascade_index = 0U;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || topology == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (topology->count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    effective = options != NULL
        ? *options
        : umi_workbench_layout_placement_options_default();

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < document->node_count &&
         status == UMI_STATUS_OK;
         ++index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[index];
        bool changed = false;
        UmiWorkbenchLayoutRect original = node->bounds;
        bool monitor_missing =
            node->monitor_id[0] != '\0' &&
            umi_workbench_monitor_topology_find(
                topology, node->monitor_id) == NULL;

        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind != UMI_WORKBENCH_LAYOUT_NODE_WINDOW &&
            node->kind !=
                UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) {
            continue;
        }

        report.inspected_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (monitor_missing) {
            report.missing_monitor_count += 1U;
        }
        status = umi_workbench_layout_place_node(
            node,
            topology,
            &effective,
            cascade_index,
            &changed);
        cascade_index += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (changed) {
            report.changed = true;
            /* Apply this branch only when its contract condition is satisfied. */
            if (original.x != node->bounds.x ||
                original.y != node->bounds.y) {
                report.relocated_count += 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (original.width != node->bounds.width ||
                original.height != node->bounds.height) {
                report.resized_count += 1U;
            }
            /* Apply this operation only while the related capability or state is available. */
            if (umi_workbench_layout_rect_is_valid(&original) &&
                effective.clamp_to_work_area) {
                report.clamped_count += 1U;
            }
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && report.changed) {
        umi_workbench_layout_document_increment_revision(document);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_report != NULL) {
        *out_report = report;
    }
    return status;
}
