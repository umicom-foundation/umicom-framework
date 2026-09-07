/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_render_plan.c
 * PURPOSE: Verify canonical Trader geometry becomes deterministic GTK-ready stacks.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"

/* Count source tab stacks in one region so this regression test follows the
 * renderer's grouping rule when panels are added or moved in the future. */
static size_t count_layout_stacks(const UmiUiWorkspaceLayout *layout,
                                  UmiUiPlacement placement)
{
    size_t count = 0U;
    size_t index;

    if (layout == NULL || layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return 0U;
    for (index = 0U; index < layout->window_count; ++index) {
        UmiUiPlacement source_placement;
        if (umi_ui_placement_parse(layout->windows[index].placement_id,
                                   &source_placement) == UMI_STATUS_OK &&
            source_placement == placement) {
            const char *stack_id = layout->windows[index].stack_id[0] != '\0'
                ? layout->windows[index].stack_id
                : (layout->windows[index].group_id[0] != '\0'
                    ? layout->windows[index].group_id
                    : umi_ui_placement_text(placement));
            int already_counted = 0;
            size_t previous;
            /* A stack is counted once even when several panels share it. */
            for (previous = 0U; previous < index; ++previous) {
                UmiUiPlacement previous_placement;
                const char *previous_stack_id;
                if (umi_ui_placement_parse(
                        layout->windows[previous].placement_id,
                        &previous_placement) != UMI_STATUS_OK ||
                    previous_placement != placement) {
                    continue;
                }
                previous_stack_id =
                    layout->windows[previous].stack_id[0] != '\0'
                        ? layout->windows[previous].stack_id
                        : (layout->windows[previous].group_id[0] != '\0'
                            ? layout->windows[previous].group_id
                            : umi_ui_placement_text(placement));
                if (strcmp(previous_stack_id, stack_id) == 0) {
                    already_counted = 1;
                    break;
                }
            }
            if (!already_counted) count += 1U;
        }
    }
    return count;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiWorkspaceLayout *layout;
    UmiApplicationSuiteLayoutRenderPlan *plan;
    /* Keep the sizeable fixed-capacity layout and render plan off the native
     * stack used by Windows test processes. */
    layout = (UmiUiWorkspaceLayout *)calloc(1U, sizeof(*layout));
    plan = (UmiApplicationSuiteLayoutRenderPlan *)calloc(1U, sizeof(*plan));
    UMI_TEST_REQUIRE(layout != NULL && plan != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_project_default(
        "org.umicom.trader", layout) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
        layout, plan) == UMI_STATUS_OK);
    /* The expected total is the projected layout size, not a frozen literal. */
    UMI_TEST_REQUIRE(plan->visible_window_count == layout->window_count);
    /* A region can contain several windows in one tab stack, so compare the
     * render plan with source stack identities instead of counting windows. */
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_count_placement(
        plan, UMI_UI_PLACEMENT_LEFT) ==
                     count_layout_stacks(layout, UMI_UI_PLACEMENT_LEFT));
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_count_placement(
        plan, UMI_UI_PLACEMENT_CENTRE) ==
                     count_layout_stacks(layout, UMI_UI_PLACEMENT_CENTRE));
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_count_placement(
        plan, UMI_UI_PLACEMENT_RIGHT) ==
                     count_layout_stacks(layout, UMI_UI_PLACEMENT_RIGHT));
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_count_placement(
        plan, UMI_UI_PLACEMENT_BOTTOM) ==
                     count_layout_stacks(layout, UMI_UI_PLACEMENT_BOTTOM));
    UMI_TEST_REQUIRE(layout->windows[0].placement_id[0] != '\0');
    UMI_TEST_REQUIRE(layout->windows[0].stack_id[0] != '\0');
    /* A linked context is independent from placement and does not change the
     * region selected by the renderer. */
    (void)snprintf(layout->windows[0].context_group_id,
                   sizeof(layout->windows[0].context_group_id), "market-red");
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
        layout, plan) == UMI_STATUS_OK);
    /* An auto-hide record belongs only to an edge rail, never also to a
     * visible dock stack or native floating window. Every supported hidden
     * edge remains valid, while malformed imported records fail early. */
    layout->window_count = 1U;
    layout->windows[0].visible = false;
    layout->windows[0].floating = false;
    const char *const hidden_edges[] = {
        "auto-hide:left", "auto-hide:right", "auto-hide:top", "auto-hide:bottom"
    };
    for (size_t edge = 0U; edge < sizeof(hidden_edges) / sizeof(hidden_edges[0]); ++edge) {
        (void)snprintf(layout->windows[0].placement_id,
            sizeof(layout->windows[0].placement_id), "%s", hidden_edges[edge]);
        UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
            layout, plan) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(plan->visible_window_count == 0U &&
            plan->stack_count == 0U && plan->canvas_item_count == 0U);
        layout->windows[0].visible = true;
        UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
            layout, plan) == UMI_STATUS_INVALID_ARGUMENT);
        layout->windows[0].visible = false;
        layout->windows[0].floating = true;
        UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
            layout, plan) == UMI_STATUS_INVALID_ARGUMENT);
        layout->windows[0].floating = false;
    }
    const char *const invalid_edges[] = {
        "auto-hide:", "auto-hide:centre", "auto-hide:floating", "auto-hide:left-extra"
    };
    for (size_t edge = 0U; edge < sizeof(invalid_edges) / sizeof(invalid_edges[0]); ++edge) {
        (void)snprintf(layout->windows[0].placement_id,
            sizeof(layout->windows[0].placement_id), "%s", invalid_edges[edge]);
        UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
            layout, plan) == UMI_STATUS_INVALID_ARGUMENT);
    }
    /* A hidden unterminated placement must never reach the prefix comparison. */
    (void)memset(layout->windows[0].placement_id, 'x',
        sizeof(layout->windows[0].placement_id));
    UMI_TEST_REQUIRE(umi_application_suite_layout_render_plan_build(
        layout, plan) == UMI_STATUS_INVALID_STATE);
    free(plan);
    free(layout);
    return 0;
}
