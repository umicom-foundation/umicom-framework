/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/render_plan.h
 *
 * PURPOSE:
 *   Convert canonical geometric Application Suite layouts into bounded region
 *   and tab-stack plans that native frontend adapters can render consistently.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiApplicationSuiteLayoutRenderStack {
    char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiUiPlacement placement;
    UmiApplicationSuiteLayoutRect rect;
    size_t window_indices[UMI_APPLICATION_SUITE_LAYOUT_STACK_MAX_WINDOWS];
    size_t window_count;
} UmiApplicationSuiteLayoutRenderStack;

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

UmiStatus umi_application_suite_layout_render_plan_build(
    const UmiUiWorkspaceLayout *layout,
    UmiApplicationSuiteLayoutRenderPlan *out_plan);
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_stack_at(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    size_t index);
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_find_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    size_t occurrence);
size_t umi_application_suite_layout_render_plan_count_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
