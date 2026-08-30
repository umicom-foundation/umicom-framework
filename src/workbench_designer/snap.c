/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/snap.c
 *
 * PURPOSE:
 *   Implement snap selection using the nearest eligible alignment within
 *   tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/snap.h"
#include "internal.h"
#include <math.h>


UmiWorkbenchDesignerSnapSettings umi_workbench_designer_snap_settings_default(void)
{
    UmiWorkbenchDesignerSnapSettings settings;
    settings.snap_to_grid = true;
    settings.snap_to_edges = true;
    settings.snap_to_centres = true;
    settings.tolerance = 6.0;
    return settings;
}

void umi_workbench_designer_snap_result_init(
    UmiWorkbenchDesignerSnapResult *result,
    UmiWorkbenchDesignerRect bounds)
{
    if (result == NULL) return;
    (void)memset(result, 0, sizeof(*result));
    result->bounds = bounds;
}

static void snap_add_guide(
    UmiWorkbenchDesignerSnapResult *result,
    bool vertical,
    double coordinate,
    const char *source_node_id)
{
    UmiWorkbenchDesignerSnapGuide *guide;
    if (result->guide_count >= UMI_WORKBENCH_DESIGNER_MAX_GUIDES) return;
    guide = &result->guides[result->guide_count++];
    guide->vertical = vertical;
    guide->coordinate = coordinate;
    guide->source_node_id[0] = '\0';
    if (source_node_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            guide->source_node_id,
            sizeof(guide->source_node_id),
            source_node_id);
    }
}

static bool snap_axis(
    double candidate_values[3],
    const double target_values[3],
    bool allow_centres,
    double tolerance,
    double *out_delta,
    double *out_coordinate)
{
    size_t candidate_index;
    size_t target_index;
    bool found = false;
    double best = tolerance + 1.0;
    for (candidate_index = 0U; candidate_index < 3U; ++candidate_index) {
        if (candidate_index == 1U && !allow_centres) continue;
        for (target_index = 0U; target_index < 3U; ++target_index) {
            double delta;
            double distance;
            if (target_index == 1U && !allow_centres) continue;
            delta = target_values[target_index] - candidate_values[candidate_index];
            distance = fabs(delta);
            if (distance <= tolerance && distance < best) {
                best = distance;
                *out_delta = delta;
                *out_coordinate = target_values[target_index];
                found = true;
            }
        }
    }
    return found;
}

UmiStatus umi_workbench_designer_snap_rect(
    const UmiWorkbenchDesignerSnapSettings *settings,
    const UmiWorkbenchDesignerGrid *grid,
    const UmiWorkbenchDesignerRect *candidate,
    const UmiWorkbenchDesignerRect *other_bounds,
    const char *const *other_ids,
    size_t other_count,
    UmiWorkbenchDesignerSnapResult *out_result)
{
    size_t index;
    double best_x = settings != NULL ? settings->tolerance + 1.0 : 0.0;
    double best_y = best_x;
    double delta_x = 0.0;
    double delta_y = 0.0;
    double coordinate_x = 0.0;
    double coordinate_y = 0.0;
    const char *source_x = NULL;
    const char *source_y = NULL;
    if (settings == NULL || candidate == NULL || out_result == NULL ||
        !umi_workbench_designer_rect_is_valid(candidate) ||
        !isfinite(settings->tolerance) || settings->tolerance < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_snap_result_init(out_result, *candidate);
    if (settings->snap_to_grid && grid != NULL && grid->enabled) {
        UmiWorkbenchDesignerPoint point = {candidate->x, candidate->y};
        UmiWorkbenchDesignerPoint snapped =
            umi_workbench_designer_grid_snap_point(grid, point);
        if (fabs(snapped.x - candidate->x) <= settings->tolerance) {
            delta_x = snapped.x - candidate->x;
            best_x = fabs(delta_x);
            coordinate_x = snapped.x;
        }
        if (fabs(snapped.y - candidate->y) <= settings->tolerance) {
            delta_y = snapped.y - candidate->y;
            best_y = fabs(delta_y);
            coordinate_y = snapped.y;
        }
    }
    if (settings->snap_to_edges && other_bounds != NULL) {
        for (index = 0U; index < other_count; ++index) {
            double candidate_x[3] = {
                candidate->x,
                candidate->x + candidate->width * 0.5,
                candidate->x + candidate->width};
            double candidate_y[3] = {
                candidate->y,
                candidate->y + candidate->height * 0.5,
                candidate->y + candidate->height};
            double target_x[3] = {
                other_bounds[index].x,
                other_bounds[index].x + other_bounds[index].width * 0.5,
                other_bounds[index].x + other_bounds[index].width};
            double target_y[3] = {
                other_bounds[index].y,
                other_bounds[index].y + other_bounds[index].height * 0.5,
                other_bounds[index].y + other_bounds[index].height};
            double candidate_delta;
            double candidate_coordinate;
            if (snap_axis(candidate_x, target_x, settings->snap_to_centres,
                          settings->tolerance, &candidate_delta,
                          &candidate_coordinate) &&
                fabs(candidate_delta) < best_x) {
                best_x = fabs(candidate_delta);
                delta_x = candidate_delta;
                coordinate_x = candidate_coordinate;
                source_x = other_ids != NULL ? other_ids[index] : NULL;
            }
            if (snap_axis(candidate_y, target_y, settings->snap_to_centres,
                          settings->tolerance, &candidate_delta,
                          &candidate_coordinate) &&
                fabs(candidate_delta) < best_y) {
                best_y = fabs(candidate_delta);
                delta_y = candidate_delta;
                coordinate_y = candidate_coordinate;
                source_y = other_ids != NULL ? other_ids[index] : NULL;
            }
        }
    }
    if (best_x <= settings->tolerance) {
        out_result->bounds.x += delta_x;
        out_result->snapped_x = true;
        snap_add_guide(out_result, true, coordinate_x, source_x);
    }
    if (best_y <= settings->tolerance) {
        out_result->bounds.y += delta_y;
        out_result->snapped_y = true;
        snap_add_guide(out_result, false, coordinate_y, source_y);
    }
    return UMI_STATUS_OK;
}
