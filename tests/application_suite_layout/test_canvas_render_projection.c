/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_canvas_render_projection.c
 *
 * PURPOSE:
 *   Verify portable free-canvas render plans and pointer geometry without a
 *   display server, preserving existing docked and native floating layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/suite_layout/render_plan.h"
#include "umicom/test_runtime/check.h"
#include "umicom/ui/workspace_customisation.h"

/* Helpers return different types, so failed checks stop the test directly
 * instead of returning an integer from a void helper. Release checks stay on. */
#define CANVAS_REQUIRE(expression) do { \
    if (!(expression)) { \
        exit(umi_test_runtime_report_failed_check(__FILE__, __LINE__, #expression)); \
    } \
} while (0)

/* Compare decimal coordinates without requiring exact binary representation. */
static void require_near(double actual, double expected)
{
    double difference = actual - expected;
    CANVAS_REQUIRE(isfinite(actual));
    CANVAS_REQUIRE(difference > -0.000000001 && difference < 0.000000001);
}

/* Start each scenario with an identified, deliberately empty workspace. */
static void reset_layout(UmiUiWorkspaceLayout *layout)
{
    (void)memset(layout, 0, sizeof(*layout));
    (void)snprintf(layout->layout_id, sizeof(layout->layout_id), "test.canvas");
    (void)snprintf(layout->name, sizeof(layout->name), "Canvas regression");
    layout->revision = 17U;
}

/* Give one panel a unique identity and a valid non-canonical rectangle. */
static void fill_canvas_window(UmiUiWorkspaceWindow *window, size_t index)
{
    (void)memset(window, 0, sizeof(*window));
    (void)snprintf(window->window_id, sizeof(window->window_id), "panel.%zu", index);
    (void)snprintf(window->tool_id, sizeof(window->tool_id), "test.tool");
    (void)snprintf(window->placement_id, sizeof(window->placement_id),
                   "%s", UMI_UI_WORKSPACE_CANVAS_PLACEMENT);
    (void)snprintf(window->stack_id, sizeof(window->stack_id), "stack.%zu", index);
    window->x = 0.1;
    window->y = 0.2;
    window->width = 0.3;
    window->height = 0.4;
    window->visible = true;
    window->resizable = true;
    window->z_order = 5;
}

/* Blank canvases and mixed presentations must not manufacture centre tabs. */
static void test_blank_and_mixed(UmiUiWorkspaceLayout *layout,
                                 UmiApplicationSuiteLayoutRenderPlan *plan)
{
    reset_layout(layout);
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_OK);
    CANVAS_REQUIRE(plan->stack_count == 0U && plan->canvas_item_count == 0U);
    CANVAS_REQUIRE(plan->visible_window_count == 0U && plan->source_revision == 17U);
    layout->window_count = 4U;
    /* The first item stays a canvas panel even when its rectangle happens to
     * match a docked region; explicit placement is the authority. */
    for (size_t index = 0U; index < layout->window_count; ++index)
        fill_canvas_window(&layout->windows[index], index);
    layout->windows[0].z_order = -3;
    (void)snprintf(layout->windows[1].placement_id,
                   sizeof(layout->windows[1].placement_id), "left");
    layout->windows[2].floating = true;
    layout->windows[3].visible = false;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_OK);
    CANVAS_REQUIRE(plan->visible_window_count == 3U);
    CANVAS_REQUIRE(plan->canvas_item_count == 1U && plan->stack_count == 2U);
    CANVAS_REQUIRE(plan->floating_window_count == 1U);
    CANVAS_REQUIRE(plan->canvas_items[0].window_index == 0U);
    CANVAS_REQUIRE(plan->canvas_items[0].z_order == -3);
    require_near(plan->canvas_items[0].rect.x, 0.1);
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_count_placement(
        plan, UMI_UI_PLACEMENT_CENTRE) == 0U);
    /* Hidden panels remain saved but contribute no visible instructions. */
    for (size_t index = 0U; index < layout->window_count; ++index)
        layout->windows[index].visible = false;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_OK);
    CANVAS_REQUIRE(plan->visible_window_count == 0U);
}

/* All 64 canvas panels fit independently of the unchanged 16 docked stacks. */
static void test_capacity(UmiUiWorkspaceLayout *layout,
                          UmiApplicationSuiteLayoutRenderPlan *plan)
{
    reset_layout(layout);
    layout->window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS;
    for (size_t index = 0U; index < layout->window_count; ++index) {
        fill_canvas_window(&layout->windows[index], index);
        layout->windows[index].z_order = (int32_t)(index % 3U);
    }
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_OK);
    CANVAS_REQUIRE(plan->canvas_item_count == UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS);
    CANVAS_REQUIRE(plan->visible_window_count == plan->canvas_item_count);
    CANVAS_REQUIRE(plan->stack_count == 0U);
    /* The plan preserves source order; the adapter sorts only its paint order. */
    for (size_t index = 0U; index < layout->window_count; ++index) {
        CANVAS_REQUIRE(plan->canvas_items[index].window_index == index);
        CANVAS_REQUIRE(plan->canvas_items[index].z_order == (int32_t)(index % 3U));
    }
    layout->window_count = UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS + 1U;
    for (size_t index = 0U; index < layout->window_count; ++index)
        (void)snprintf(layout->windows[index].placement_id,
                       sizeof(layout->windows[index].placement_id), "left");
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_CAPACITY_EXCEEDED);
    layout->window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
}

/* Bad saved geometry and unterminated identities must stop before GTK reads it. */
static void test_invalid_canvas(UmiUiWorkspaceLayout *layout,
                                UmiApplicationSuiteLayoutRenderPlan *plan)
{
    const double invalid_coordinates[] = {NAN, INFINITY, -INFINITY, -0.01, 1.01};
    reset_layout(layout);
    layout->window_count = 1U;
    for (size_t index = 0U;
         index < sizeof(invalid_coordinates) / sizeof(invalid_coordinates[0]);
         ++index) {
        /* Exercise every rectangle field, not only its position. */
        for (unsigned int field = 0U; field < 4U; ++field) {
            fill_canvas_window(&layout->windows[0], 0U);
            if (field == 0U) layout->windows[0].x = invalid_coordinates[index];
            if (field == 1U) layout->windows[0].y = invalid_coordinates[index];
            if (field == 2U) layout->windows[0].width = invalid_coordinates[index];
            if (field == 3U) layout->windows[0].height = invalid_coordinates[index];
            CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(
                layout, plan) == UMI_STATUS_INVALID_STATE);
        }
    }
    fill_canvas_window(&layout->windows[0], 0U);
    layout->windows[0].width = 0.0;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    fill_canvas_window(&layout->windows[0], 0U);
    layout->windows[0].x = 0.8;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    fill_canvas_window(&layout->windows[0], 0U);
    (void)memset(layout->windows[0].placement_id, 'x',
                  sizeof(layout->windows[0].placement_id));
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    reset_layout(layout);
    /* A hidden record still owns its instance ID. Accepting a duplicate could
     * attach one retained editor body to two different panel wrappers. */
    layout->window_count = 2U;
    fill_canvas_window(&layout->windows[0], 0U);
    layout->windows[1] = layout->windows[0];
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    layout->windows[1].visible = false;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    fill_canvas_window(&layout->windows[1], 1U);
    layout->windows[1].visible = false;
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_OK);
    /* Hidden tool identities are read during cache replacement, so their
     * strings need the same termination check as visible tool identities. */
    (void)memset(layout->windows[1].tool_id, 'x', sizeof(layout->windows[1].tool_id));
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
    reset_layout(layout);
    layout->layout_id[0] = '\0';
    CANVAS_REQUIRE(umi_application_suite_layout_render_plan_build(layout, plan) ==
                     UMI_STATUS_INVALID_STATE);
}

/* Movement, resizing and snapping all use the original gesture rectangle. */
static void test_gestures(void)
{
    UmiApplicationSuiteLayoutRect start = {0.1, 0.2, 0.3, 0.4};
    UmiApplicationSuiteLayoutRect result;
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
    require_near(result.x, 0.2);
    require_near(result.y, 0.3);
    require_near(result.width, start.width);
    require_near(result.height, start.height);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        DBL_MAX, -DBL_MAX, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
    require_near(result.x, 0.7);
    require_near(result.y, 0.0);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        65.0, 40.0, 1000.0, 500.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    require_near(result.x, 0.2);
    require_near(result.y, 0.3);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST,
        100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
    require_near(result.x, start.x);
    require_near(result.y, start.y);
    require_near(result.width, 0.4);
    require_near(result.height, 0.5);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST,
        -DBL_MAX, -DBL_MAX, 1000.0, 500.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    require_near(result.width, 0.12);
    require_near(result.height, 0.16);
    /* A tiny host clips impossible minimums to the available viewport. */
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST,
        DBL_MAX, DBL_MAX, 20.0, 10.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    require_near(result.width, 0.9);
    require_near(result.height, 0.8);
    CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
    /* An unrepresentably small grid is harmless and does not overflow. */
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        0.0, 0.0, 1000.0, 500.0, 120.0, 80.0, DBL_MIN, &start) == UMI_STATUS_OK);
    require_near(start.x, 0.1);
    require_near(start.y, 0.2);
    /* Sub-precision panel sizes still stop before x/y reach exactly one. */
    start.width = DBL_MIN;
    start.height = DBL_MIN;
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        DBL_MAX, DBL_MAX, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
}

/* Every edge and corner uses the same pixel basis and keeps its opposite
 * anchor. Explicit expected rectangles catch reversed north/west signs. */
static void test_resize_directions(void)
{
    static const struct {
        UmiApplicationSuiteLayoutCanvasGesture gesture;
        int horizontal;
        int vertical;
        UmiApplicationSuiteLayoutRect expected;
    } cases[] = {
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST, 1, 1, {0.2, 0.25, 0.5, 0.6}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH, 0, -1, {0.2, 0.35, 0.4, 0.4}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_EAST, 1, -1, {0.2, 0.35, 0.5, 0.4}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_EAST, 1, 0, {0.2, 0.25, 0.5, 0.5}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH, 0, 1, {0.2, 0.25, 0.4, 0.6}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_WEST, -1, 1, {0.3, 0.25, 0.3, 0.6}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_WEST, -1, 0, {0.3, 0.25, 0.3, 0.5}},
        {UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST, -1, -1, {0.3, 0.35, 0.3, 0.4}}
    };
    const UmiApplicationSuiteLayoutRect start = {0.2, 0.25, 0.4, 0.5};
    const UmiApplicationSuiteLayoutRect off_grid = {0.13, 0.17, 0.04, 0.05};
    UmiApplicationSuiteLayoutRect result;
    CANVAS_REQUIRE(UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE == 1 &&
        UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST == 2);
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        const int horizontal = cases[index].horizontal;
        const int vertical = cases[index].vertical;
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, cases[index].gesture, 100.0, 50.0, 1000.0, 500.0,
            120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
        require_near(result.x, cases[index].expected.x);
        require_near(result.y, cases[index].expected.y);
        require_near(result.width, cases[index].expected.width);
        require_near(result.height, cases[index].expected.height);
        CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
        /* The caller can safely reuse its original rectangle as output. */
        result = start;
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &result, cases[index].gesture, 100.0, 50.0, 1000.0, 500.0,
            120.0, 80.0, 0.0, &result) == UMI_STATUS_OK);
        require_near(result.x, cases[index].expected.x);
        require_near(result.height, cases[index].expected.height);

        /* A coarse grid cannot override the minimum. Huge inward gestures
         * stop before crossing the fixed edge or producing an empty panel. */
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, cases[index].gesture, horizontal < 0 ? DBL_MAX : -DBL_MAX,
            vertical < 0 ? DBL_MAX : -DBL_MAX, 1000.0, 500.0,
            120.0, 80.0, 0.3, &result) == UMI_STATUS_OK);
        require_near(result.width, horizontal == 0 ? start.width : 0.12);
        require_near(result.height, vertical == 0 ? start.height : 0.16);
        require_near(horizontal < 0 ? result.x + result.width : result.x,
            horizontal < 0 ? start.x + start.width : start.x);
        require_near(vertical < 0 ? result.y + result.height : result.y,
            vertical < 0 ? start.y + start.height : start.y);
        CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));

        /* Outward gestures stop at the viewport, even when an impossible
         * minimum and grid would prefer a larger panel. */
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, cases[index].gesture, horizontal < 0 ? -DBL_MAX : DBL_MAX,
            vertical < 0 ? -DBL_MAX : DBL_MAX, 20.0, 10.0,
            120.0, 80.0, 0.3, &result) == UMI_STATUS_OK);
        require_near(result.width, horizontal == 0 ? start.width
            : horizontal < 0 ? start.x + start.width : 1.0 - start.x);
        require_near(result.height, vertical == 0 ? start.height
            : vertical < 0 ? start.y + start.height : 1.0 - start.y);
        require_near(horizontal < 0 ? result.x + result.width : result.x,
            horizontal < 0 ? start.x + start.width : start.x);
        require_near(vertical < 0 ? result.y + result.height : result.y,
            vertical < 0 ? start.y + start.height : start.y);
        CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));

        /* Imported undersized panels do not jump on pointer-down, and an
         * untouched axis remains byte-for-byte unchanged despite snapping. */
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &off_grid, cases[index].gesture, 0.0, 0.0, 1000.0, 500.0,
            120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
        CANVAS_REQUIRE(memcmp(&result, &off_grid, sizeof(result)) == 0);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &off_grid, cases[index].gesture, 50.0, 0.0, 1000.0, 500.0,
            120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
        CANVAS_REQUIRE(result.y == off_grid.y && result.height == off_grid.height);
        if (horizontal == 0) CANVAS_REQUIRE(memcmp(&result, &off_grid, sizeof(result)) == 0);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &off_grid, cases[index].gesture, 0.0, 50.0, 1000.0, 500.0,
            120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
        CANVAS_REQUIRE(result.x == off_grid.x && result.width == off_grid.width);
        if (vertical == 0) CANVAS_REQUIRE(memcmp(&result, &off_grid, sizeof(result)) == 0);
    }
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &off_grid, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE, 0.0, 0.0,
        1000.0, 500.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    CANVAS_REQUIRE(memcmp(&result, &off_grid, sizeof(result)) == 0);
}

/* North/west dimension snapping retains the original opposite edge instead
 * of snapping that edge or changing the unrelated dimension. */
static void test_resize_snapping_and_precision(void)
{
    UmiApplicationSuiteLayoutRect start = {0.13, 0.17, 0.32, 0.39};
    UmiApplicationSuiteLayoutRect result;
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
        46.0, 31.0, 1000.0, 500.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    require_near(result.width, 0.3); require_near(result.height, 0.3);
    require_near(result.x, 0.15); require_near(result.y, 0.26);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
        46.0, 31.0, 1000.0, 500.0, 120.0, 80.0, DBL_MIN, &result) == UMI_STATUS_OK);
    require_near(result.width, 0.274); require_near(result.height, 0.328);
    require_near(result.x + result.width, start.x + start.width);
    require_near(result.y + result.height, start.y + start.height);
    /* The minimum must not exceed a subnormal amount of available space. */
    start = (UmiApplicationSuiteLayoutRect){0.0, 0.0, DBL_MIN / 2.0, DBL_MIN / 2.0};
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
        DBL_MAX, DBL_MAX, 1.0, 1.0, 120.0, 80.0, 0.1, &result) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
    CANVAS_REQUIRE(result.x == 0.0 && result.y == 0.0 &&
        result.width == start.width && result.height == start.height);
    start = (UmiApplicationSuiteLayoutRect){0.5, 0.5, 0.5, 0.5};
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
        DBL_MAX, DBL_MAX, 1.0, 1.0, DBL_MIN, DBL_MIN, 0.0, &result) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
    require_near(result.x + result.width, 1.0);
    require_near(result.y + result.height, 1.0);
}

/* Invalid motion inputs leave the caller's previous preview untouched. */
static void test_invalid_gestures(void)
{
    UmiApplicationSuiteLayoutRect start = {0.1, 0.2, 0.3, 0.4};
    UmiApplicationSuiteLayoutRect result = start;
    const double invalid_values[] = {0.0, -1.0, NAN, INFINITY};
    for (size_t index = 0U;
         index < sizeof(invalid_values) / sizeof(invalid_values[0]); ++index) {
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
            100.0, 50.0, invalid_values[index], 500.0, 120.0, 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(memcmp(&result, &start, sizeof(result)) == 0);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
            100.0, 50.0, 1000.0, invalid_values[index], 120.0, 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_EAST,
            100.0, 50.0, 1000.0, 500.0, invalid_values[index], 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_WEST,
            100.0, 50.0, 1000.0, 500.0, 120.0, invalid_values[index], 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(memcmp(&result, &start, sizeof(result)) == 0);
    }
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        NAN, 0.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, (UmiApplicationSuiteLayoutCanvasGesture)99,
        0.0, 0.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, (UmiApplicationSuiteLayoutCanvasGesture)0,
        0.0, 0.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_WEST,
        INFINITY, 0.0, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH,
        0.0, -INFINITY, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH,
        0.0, NAN, 1000.0, 500.0, 120.0, 80.0, 0.0, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
        &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE,
        0.0, 0.0, 1000.0, 500.0, 120.0, 80.0, 1.1, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(memcmp(&result, &start, sizeof(result)) == 0);
    {
        const double invalid_grids[] = {-0.1, NAN, INFINITY};
        UmiApplicationSuiteLayoutRect invalid_start = start;
        for (size_t index = 0U; index < sizeof(invalid_grids) / sizeof(invalid_grids[0]); ++index)
            CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
                &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_EAST,
                100.0, 0.0, 1000.0, 500.0, 120.0, 80.0, invalid_grids[index],
                &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            NULL, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
            100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
            100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0,
            NULL) == UMI_STATUS_INVALID_ARGUMENT);
        invalid_start.width = 0.0;
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &invalid_start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
            100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        invalid_start.width = NAN;
        CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
            &invalid_start, UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST,
            100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0,
            &result) == UMI_STATUS_INVALID_ARGUMENT);
        CANVAS_REQUIRE(memcmp(&result, &start, sizeof(result)) == 0);
    }
    CANVAS_REQUIRE(!umi_application_suite_layout_canvas_rect_valid(NULL));
    /* A positive but sub-precision width must not make x == 1 appear valid. */
    start.x = 1.0;
    start.width = DBL_MIN;
    CANVAS_REQUIRE(!umi_application_suite_layout_canvas_rect_valid(&start));
}

/* A fixed range of fractions exercises rounding for all nine gesture modes. */
static void test_gesture_boundary_range(void)
{
    for (unsigned int index = 0U; index < 1000U; ++index) {
        UmiApplicationSuiteLayoutRect start = {
            (double)(index % 80U) / 100.0,
            (double)((index * 7U) % 80U) / 100.0,
            0.1, 0.1
        };
        UmiApplicationSuiteLayoutRect result;
        double dx = (double)(index % 23U) * 100.0 - 1100.0;
        double dy = (double)(index % 17U) * 100.0 - 800.0;
        double grid = index % 2U == 0U ? 0.05 : 0.0;
        for (int gesture = UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE;
             gesture <= UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST; ++gesture) {
            CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
                &start, (UmiApplicationSuiteLayoutCanvasGesture)gesture,
                dx, dy, 1024.0, 768.0, 120.0, 80.0,
                grid, &result) == UMI_STATUS_OK);
            CANVAS_REQUIRE(umi_application_suite_layout_canvas_rect_valid(&result));
        }
    }
}

/* Opening and settings share the same canvas transaction and context model. */
static void test_canvas_open_and_settings(void)
{
    UmiUiWorkspaceCustomisation *workspace = calloc(1U, sizeof(*workspace));
    UmiUiWorkspaceCustomisation *before = calloc(1U, sizeof(*before));
    UmiUiWindowDescriptor descriptor = {
        .tool_id = "test.canvas.tool",
        .title = "Canvas tool",
        .category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,
        .supports_multiple = true,
        .default_width = 0.5,
        .default_height = 0.4
    };
    UmiUiWorkspacePanelSettings settings;
    const UmiUiWorkspaceWindow *window;
    char first_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char second_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    CANVAS_REQUIRE(workspace != NULL && before != NULL);
    umi_ui_workspace_customisation_init(workspace);
    CANVAS_REQUIRE(umi_ui_window_catalogue_register(&workspace->windows,
        &descriptor) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(
        workspace, "canvas.settings", "Canvas settings") == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_begin_edit(workspace) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_open_window(workspace,
        descriptor.tool_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT, false, 42U,
        first_id, sizeof(first_id)) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_open_window(workspace,
        descriptor.tool_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT, false, 43U,
        second_id, sizeof(second_id)) == UMI_STATUS_OK);
    CANVAS_REQUIRE(strcmp(first_id, second_id) != 0);
    window = umi_ui_workspace_layout_find_window(
        umi_ui_workspace_customisation_active_const(workspace), first_id);
    CANVAS_REQUIRE(window != NULL);
    CANVAS_REQUIRE(strcmp(window->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0);
    CANVAS_REQUIRE(strcmp(window->stack_id, first_id) == 0);
    CANVAS_REQUIRE(strcmp(window->group_id, first_id) == 0);
    CANVAS_REQUIRE(!window->floating);
    require_near(window->width, descriptor.default_width);
    require_near(window->height, descriptor.default_height);
    require_near(window->x, 0.25);
    require_near(window->y, 0.3);
    CANVAS_REQUIRE(umi_ui_window_group_define(&workspace->groups,
        "test.context", "blue", UMI_UI_WINDOW_CONTEXT_DOCUMENT) == UMI_STATUS_OK);

    /* Coordinates and linked group are published together, while the edit
     * baseline stays intact so the entire session can still be cancelled. */
    settings = umi_ui_workspace_panel_settings_default(first_id);
    settings.placement_id = UMI_UI_WORKSPACE_CANVAS_PLACEMENT;
    settings.stack_id = first_id;
    settings.context_group_id = "test.context";
    settings.x = 0.2;
    settings.y = 0.1;
    settings.width = 0.4;
    settings.height = 0.3;
    CANVAS_REQUIRE(umi_ui_workspace_customisation_apply_panel_settings(
        workspace, &settings) == UMI_STATUS_OK);
    window = umi_ui_workspace_layout_find_window(
        umi_ui_workspace_customisation_active_const(workspace), first_id);
    CANVAS_REQUIRE(window != NULL);
    require_near(window->x, settings.x);
    require_near(window->y, settings.y);
    require_near(window->width, settings.width);
    require_near(window->height, settings.height);
    CANVAS_REQUIRE(strcmp(window->context_group_id, "test.context") == 0);
    CANVAS_REQUIRE(workspace->groups.items[0].member_count == 1U);
    CANVAS_REQUIRE(workspace->edit_active);
    *before = *workspace;

    /* A missing context must undo the staged geometry, not leave a half edit. */
    settings.x = 0.3;
    settings.context_group_id = "missing.context";
    CANVAS_REQUIRE(umi_ui_workspace_customisation_apply_panel_settings(
        workspace, &settings) == UMI_STATUS_NOT_FOUND);
    CANVAS_REQUIRE(memcmp(before, workspace, sizeof(*workspace)) == 0);
    settings.context_group_id = "test.context";
    settings.auto_hidden = true;
    CANVAS_REQUIRE(umi_ui_workspace_customisation_apply_panel_settings(
        workspace, &settings) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_set_auto_hidden(
        workspace, first_id, true) == UMI_STATUS_INVALID_STATE);
    CANVAS_REQUIRE(memcmp(before, workspace, sizeof(*workspace)) == 0);
    settings.auto_hidden = false;
    settings.width = NAN;
    CANVAS_REQUIRE(umi_ui_workspace_customisation_apply_panel_settings(
        workspace, &settings) == UMI_STATUS_INVALID_ARGUMENT);
    CANVAS_REQUIRE(memcmp(before, workspace, sizeof(*workspace)) == 0);

    /* Projection is a pure calculation, never permission to publish it. Every
     * resize direction still passes the existing pin, resize and lock checks. */
    {
        UmiUiWorkspaceLayout *active = umi_ui_workspace_customisation_active(workspace);
        UmiUiWorkspaceWindow *editable = umi_ui_workspace_layout_find_window_mutable(active, first_id);
        UmiApplicationSuiteLayoutRect start;
        CANVAS_REQUIRE(editable != NULL);
        start = (UmiApplicationSuiteLayoutRect){editable->x, editable->y, editable->width, editable->height};
        for (int protection = 0; protection < 3; ++protection) {
            editable->pinned = protection == 0;
            editable->resizable = protection != 1;
            active->locked = protection == 2;
            *before = *workspace;
            for (int gesture = UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST;
                 gesture <= UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST; ++gesture) {
                UmiApplicationSuiteLayoutRect projected;
                CANVAS_REQUIRE(umi_application_suite_layout_project_canvas_gesture(
                    &start, (UmiApplicationSuiteLayoutCanvasGesture)gesture,
                    100.0, 50.0, 1000.0, 500.0, 120.0, 80.0, 0.0,
                    &projected) == UMI_STATUS_OK);
                CANVAS_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(workspace,
                    first_id, projected.x, projected.y, projected.width, projected.height)
                    == UMI_STATUS_PERMISSION_DENIED);
                CANVAS_REQUIRE(memcmp(before, workspace, sizeof(*workspace)) == 0);
            }
        }
        editable->pinned = false;
        editable->resizable = true;
        active->locked = false;
    }
    CANVAS_REQUIRE(umi_ui_workspace_customisation_cancel_edit(workspace) == UMI_STATUS_OK);
    CANVAS_REQUIRE(umi_ui_workspace_customisation_active_const(workspace)->window_count == 0U);
    CANVAS_REQUIRE(workspace->groups.count == 0U && !workspace->edit_active);
    free(before);
    free(workspace);
}

/* Keep sizeable layout arrays on the heap so Windows tests use a small stack. */
int main(void)
{
    UmiUiWorkspaceLayout *layout = calloc(1U, sizeof(*layout));
    UmiApplicationSuiteLayoutRenderPlan *plan = calloc(1U, sizeof(*plan));
    CANVAS_REQUIRE(layout != NULL && plan != NULL);
    test_blank_and_mixed(layout, plan);
    test_capacity(layout, plan);
    test_invalid_canvas(layout, plan);
    test_gestures();
    test_resize_directions();
    test_resize_snapping_and_precision();
    test_invalid_gestures();
    test_gesture_boundary_range();
    test_canvas_open_and_settings();
    free(plan);
    free(layout);
    return 0;
}
