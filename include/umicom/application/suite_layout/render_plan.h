/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/render_plan.h
 *
 * PURPOSE:
 *   Convert canonical geometric Application Suite layouts into bounded region
 *   and tab-stack plans that native frontend adapters can render consistently.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_RENDER_PLAN_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_RENDER_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/ui/workspace_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS 16U
#define UMI_APPLICATION_SUITE_LAYOUT_STACK_MAX_WINDOWS \
    UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS

/**
 * One renderable tab stack and the workspace-window records assigned to it.
 *
 * `window_indices` refer to the source `UmiUiWorkspaceLayout` used to build
 * the containing plan. They are indexes, not owned pointers.
 */
typedef struct UmiApplicationSuiteLayoutRenderStack {
    char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiUiPlacement placement;
    UmiApplicationSuiteLayoutRect rect;
    size_t window_indices[UMI_APPLICATION_SUITE_LAYOUT_STACK_MAX_WINDOWS];
    size_t window_count;
} UmiApplicationSuiteLayoutRenderStack;

/**
 * Bounded frontend-neutral instructions for rendering one workspace layout.
 *
 * Native adapters read this value and create toolkit widgets. The plan owns
 * all strings, geometry and indexes inside its fixed-capacity arrays.
 */
typedef struct UmiApplicationSuiteLayoutRenderPlan {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    UmiApplicationSuiteLayoutRenderStack
        stacks[UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS];
    size_t stack_count;
    size_t visible_window_count;
    size_t floating_window_count;
    uint64_t source_revision;
} UmiApplicationSuiteLayoutRenderPlan;

/**
 * Builds region and tab-stack instructions from a workspace layout.
 *
 * @param layout Borrowed source layout containing window placement state.
 * @param out_plan Receives an owned bounded render plan.
 * @return `UMI_STATUS_OK` when every visible window fits a valid stack.
 */
UmiStatus umi_application_suite_layout_render_plan_build(
    const UmiUiWorkspaceLayout *layout,
    UmiApplicationSuiteLayoutRenderPlan *out_plan);
/**
 * Returns one render stack by its zero-based position.
 *
 * @param plan Previously built render plan.
 * @param index Zero-based stack position.
 * @return A pointer owned by `plan`, or `NULL` when outside its bounds.
 */
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_stack_at(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    size_t index);
/**
 * Finds a particular occurrence of a placement in render order.
 *
 * @param plan Previously built render plan.
 * @param placement Region to search for.
 * @param occurrence Zero selects the first matching stack, one the second.
 * @return A pointer owned by `plan`, or `NULL` when no occurrence exists.
 */
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_find_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    size_t occurrence);
/**
 * Counts stacks rendered in one placement region.
 *
 * @param plan Previously built render plan.
 * @param placement Region whose stacks will be counted.
 * @return Number of matching stacks, or zero for a missing plan.
 */
size_t umi_application_suite_layout_render_plan_count_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
