/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/render_plan.c
 *
 * PURPOSE:
 *   Build deterministic frontend render plans from the existing canonical
 *   workspace layout model without introducing another docking/layout engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/render_plan.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

static int rect_matches(const UmiUiWorkspaceWindow *window,
                        UmiApplicationSuiteLayoutRect rect)
{
    const double tolerance = 0.00001;
    return absolute_value(window->x - rect.x) <= tolerance &&
           absolute_value(window->y - rect.y) <= tolerance &&
           absolute_value(window->width - rect.width) <= tolerance &&
           absolute_value(window->height - rect.height) <= tolerance;
}

static UmiStatus resolve_placement(const UmiUiWorkspaceWindow *window,
                                   UmiUiPlacement *out_placement)
{
    UmiUiPlacement placement;
    int placement_value;
    if (window == NULL || out_placement == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (window->floating) {
        *out_placement = UMI_UI_PLACEMENT_FLOATING;
        return UMI_STATUS_OK;
    }
    if (window->group_id[0] != '\0' &&
        umi_ui_placement_parse(window->group_id, &placement) == UMI_STATUS_OK) {
        *out_placement = placement;
        return UMI_STATUS_OK;
    }
    for (placement_value = (int)UMI_UI_PLACEMENT_LEFT;
         placement_value <= (int)UMI_UI_PLACEMENT_FLOATING;
         ++placement_value) {
        placement = (UmiUiPlacement)placement_value;
        if (rect_matches(window,
                         umi_application_suite_layout_region_rect(placement))) {
            *out_placement = placement;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INVALID_STATE;
}

static UmiApplicationSuiteLayoutRenderStack *find_stack(
    UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    const char *group_id)
{
    size_t index;
    for (index = 0U; index < plan->stack_count; ++index) {
        UmiApplicationSuiteLayoutRenderStack *stack = &plan->stacks[index];
        if (stack->placement == placement &&
            strcmp(stack->group_id, group_id) == 0)
            return stack;
    }
    return NULL;
}

UmiStatus umi_application_suite_layout_render_plan_build(
    const UmiUiWorkspaceLayout *layout,
    UmiApplicationSuiteLayoutRenderPlan *out_plan)
{
    size_t index;
    UmiStatus status;
    if (layout == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    status = copy_text(out_plan->layout_id, sizeof(out_plan->layout_id),
                       layout->layout_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(out_plan->name, sizeof(out_plan->name), layout->name);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        UmiApplicationSuiteLayoutRenderStack *stack;
        UmiUiPlacement placement;
        char derived_group[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        const char *group_id = window->group_id;
        if (!window->visible) continue;
        if (window->window_id[0] == '\0' || window->tool_id[0] == '\0')
            return UMI_STATUS_INVALID_STATE;
        status = resolve_placement(window, &placement);
        if (status != UMI_STATUS_OK) return status;
        if (group_id[0] == '\0') {
            int written = snprintf(derived_group, sizeof(derived_group), "%s",
                                   umi_ui_placement_text(placement));
            if (written < 0 || (size_t)written >= sizeof(derived_group))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            group_id = derived_group;
        }
        stack = find_stack(out_plan, placement, group_id);
        if (stack == NULL) {
            if (out_plan->stack_count >=
                UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            stack = &out_plan->stacks[out_plan->stack_count++];
            status = copy_text(stack->group_id, sizeof(stack->group_id), group_id);
            if (status != UMI_STATUS_OK) return status;
            stack->placement = placement;
            stack->rect.x = window->x;
            stack->rect.y = window->y;
            stack->rect.width = window->width;
            stack->rect.height = window->height;
        } else if (!rect_matches(window, stack->rect)) {
            return UMI_STATUS_INVALID_STATE;
        }
        if (stack->window_count >=
            UMI_APPLICATION_SUITE_LAYOUT_STACK_MAX_WINDOWS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        stack->window_indices[stack->window_count++] = index;
        out_plan->visible_window_count += 1U;
        if (placement == UMI_UI_PLACEMENT_FLOATING)
            out_plan->floating_window_count += 1U;
    }
    if (out_plan->visible_window_count == 0U)
        return UMI_STATUS_INVALID_STATE;
    out_plan->source_revision = layout->revision;
    return UMI_STATUS_OK;
}

const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_stack_at(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    size_t index)
{
    return plan != NULL && index < plan->stack_count
        ? &plan->stacks[index] : NULL;
}

const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_find_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    size_t occurrence)
{
    size_t index;
    size_t found = 0U;
    if (plan == NULL) return NULL;
    for (index = 0U; index < plan->stack_count; ++index) {
        if (plan->stacks[index].placement != placement) continue;
        if (found == occurrence) return &plan->stacks[index];
        found += 1U;
    }
    return NULL;
}

size_t umi_application_suite_layout_render_plan_count_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement)
{
    size_t index;
    size_t count = 0U;
    if (plan == NULL) return 0U;
    for (index = 0U; index < plan->stack_count; ++index)
        if (plan->stacks[index].placement == placement) count += 1U;
    return count;
}
