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
#include "umicom/ui/workspace_customisation.h"

#include <stdio.h>
#include <string.h>

/* Copy one required label or identifier without accepting truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /* The render plan owns its strings and needs valid writable storage. */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    /* A partial group ID could merge unrelated tab stacks. */
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* Return a non-negative magnitude without depending on a math library. */
static double absolute_value(double value)
{
    return value < 0.0 ? -value : value;
}

/* Compare normalized rectangles with a small tolerance for stored decimals. */
static int rect_matches(const UmiUiWorkspaceWindow *window,
                        UmiApplicationSuiteLayoutRect rect)
{
    const double tolerance = 0.00001;
    return absolute_value(window->x - rect.x) <= tolerance &&
           absolute_value(window->y - rect.y) <= tolerance &&
           absolute_value(window->width - rect.width) <= tolerance &&
           absolute_value(window->height - rect.height) <= tolerance;
}

/* Resolve placement from explicit state, compatibility state or geometry. */
static UmiStatus resolve_placement(const UmiUiWorkspaceWindow *window,
                                   UmiUiPlacement *out_placement)
{
    UmiUiPlacement placement;
    int placement_value;
    /* Both source state and writable output are required. */
    if (window == NULL || out_placement == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A detached window always belongs to the floating render region. */
    if (window->floating) {
        *out_placement = UMI_UI_PLACEMENT_FLOATING;
        return UMI_STATUS_OK;
    }
    /* New saved layouts carry an explicit placement identifier. */
    if (window->placement_id[0] != '\0' &&
        umi_ui_placement_parse(window->placement_id, &placement) ==
            UMI_STATUS_OK) {
        *out_placement = placement;
        return UMI_STATUS_OK;
    }
    /* group_id was the placement value in the first public model.  Keep this
     * fallback so saved layouts remain readable while new layouts use the
     * explicit placement_id field. */
    if (window->group_id[0] != '\0' &&
        umi_ui_placement_parse(window->group_id, &placement) == UMI_STATUS_OK) {
        *out_placement = placement;
        return UMI_STATUS_OK;
    }
    /* Last-resort geometry inference keeps older portable layouts readable. */
    for (placement_value = (int)UMI_UI_PLACEMENT_LEFT;
         placement_value <= (int)UMI_UI_PLACEMENT_FLOATING;
         ++placement_value) {
        placement = (UmiUiPlacement)placement_value;
        /* The first canonical rectangle match supplies the missing region. */
        if (rect_matches(window,
                         umi_application_suite_layout_region_rect(placement))) {
            *out_placement = placement;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INVALID_STATE;
}

/* Find an existing stack with the same placement and stable group identity. */
static UmiApplicationSuiteLayoutRenderStack *find_stack(
    UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    const char *group_id)
{
    size_t index;
    /* Search only the stacks already built into the bounded plan. */
    for (index = 0U; index < plan->stack_count; ++index) {
        UmiApplicationSuiteLayoutRenderStack *stack = &plan->stacks[index];
        /* Both region and group must match before windows may share tabs. */
        if (stack->placement == placement &&
            strcmp(stack->group_id, group_id) == 0)
            return stack;
    }
    return NULL;
}

/* Convert editable window state into deterministic frontend instructions. */
UmiStatus umi_application_suite_layout_render_plan_build(
    const UmiUiWorkspaceLayout *layout,
    UmiApplicationSuiteLayoutRenderPlan *out_plan)
{
    size_t index;
    UmiStatus status;
    /* Rendering needs a source layout and caller-owned result storage. */
    if (layout == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The workspace stores windows in a fixed array. Reject a corrupted count
     * before the projection loop could index beyond that array. */
    if (layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_INVALID_STATE;
    /* Fixed arrays loaded from saved state must contain terminated strings.
     * A blank canvas still needs a stable identity before it can be saved. */
    if (layout->layout_id[0] == '\0' ||
        memchr(layout->layout_id, '\0', sizeof(layout->layout_id)) == NULL ||
        memchr(layout->name, '\0', sizeof(layout->name)) == NULL)
        return UMI_STATUS_INVALID_STATE;
    /* An instance identity must resolve to one record, including hidden
     * records. Duplicates could otherwise select the wrong panel or mount a
     * retained provider widget in two different parents at the same time. */
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        if (memchr(window->window_id, '\0', sizeof(window->window_id)) == NULL ||
            memchr(window->tool_id, '\0', sizeof(window->tool_id)) == NULL ||
            memchr(window->placement_id, '\0', sizeof(window->placement_id)) == NULL)
            return UMI_STATUS_INVALID_STATE;
        /* Auto-hide is a docked-only saved presentation. Validate hidden
         * records too: otherwise an imported tool could be mounted twice or
         * saved with an edge that no native renderer can display. */
        if (strncmp(window->placement_id, "auto-hide:", sizeof("auto-hide:") - 1U) == 0) {
            const char *edge = window->placement_id + sizeof("auto-hide:") - 1U;
            if (window->visible || window->floating ||
                (strcmp(edge, "left") != 0 && strcmp(edge, "right") != 0 &&
                 strcmp(edge, "top") != 0 && strcmp(edge, "bottom") != 0))
                return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (window->window_id[0] == '\0') continue;
        for (size_t previous = 0U; previous < index; ++previous) {
            if (strcmp(layout->windows[previous].window_id, window->window_id) == 0)
                return UMI_STATUS_INVALID_STATE;
        }
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    status = copy_text(out_plan->layout_id, sizeof(out_plan->layout_id),
                       layout->layout_id);
    /* Copy the name only after the stable layout ID fits completely. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_plan->name, sizeof(out_plan->name), layout->name);
    /* Do not publish a plan with partial identity. */
    if (status != UMI_STATUS_OK) return status;

    /* Visible windows are assigned to a render stack in source order. */
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        UmiApplicationSuiteLayoutRenderStack *stack;
        UmiUiPlacement placement;
        char derived_group[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        const char *group_id = window->stack_id[0] != '\0'
            ? window->stack_id : window->group_id;
        /* Hidden windows remain in workspace state but need no native widget. */
        if (!window->visible) continue;
        /* Renderable windows require both instance and reusable tool identity. */
        if (window->window_id[0] == '\0' || window->tool_id[0] == '\0')
            return UMI_STATUS_INVALID_STATE;
        /* Validate bounded text before any placement comparison or copy can
         * read past a malformed saved record's fixed-capacity fields. */
        if (memchr(window->window_id, '\0', sizeof(window->window_id)) == NULL ||
            memchr(window->tool_id, '\0', sizeof(window->tool_id)) == NULL ||
            memchr(window->placement_id, '\0', sizeof(window->placement_id)) == NULL ||
            memchr(window->stack_id, '\0', sizeof(window->stack_id)) == NULL ||
            memchr(window->group_id, '\0', sizeof(window->group_id)) == NULL)
            return UMI_STATUS_INVALID_STATE;
        /* Free-positioned panels belong directly to the canvas. Native
         * floating state takes precedence when a previously free panel has
         * been detached into its own operating-system window. */
        if (!window->floating &&
            strcmp(window->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0) {
            UmiApplicationSuiteLayoutRect rect = {
                window->x, window->y, window->width, window->height
            };
            UmiApplicationSuiteLayoutCanvasItem *item;
            /* Normalized geometry must be finite, positive and fully inside
             * the viewport before any adapter converts it into pixels. */
            if (!umi_application_suite_layout_canvas_rect_valid(&rect))
                return UMI_STATUS_INVALID_STATE;
            if (out_plan->canvas_item_count >=
                UMI_APPLICATION_SUITE_LAYOUT_MAX_CANVAS_ITEMS)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            item = &out_plan->canvas_items[out_plan->canvas_item_count++];
            item->window_index = index;
            item->rect = rect;
            item->z_order = window->z_order;
            out_plan->visible_window_count += 1U;
            continue;
        }
        status = resolve_placement(window, &placement);
        /* A window without a reliable region cannot enter a deterministic UI. */
        if (status != UMI_STATUS_OK) return status;
        /* An absent stack ID uses the resolved region as a stable default. */
        if (group_id[0] == '\0') {
            int written = snprintf(derived_group, sizeof(derived_group), "%s",
                                   umi_ui_placement_text(placement));
            /* Reject the derived group if its full name does not fit. */
            if (written < 0 || (size_t)written >= sizeof(derived_group))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            group_id = derived_group;
        }
        stack = find_stack(out_plan, placement, group_id);
        /* Create a new stack only when no compatible stack exists. */
        if (stack == NULL) {
            /* Bounded storage protects the plan from excessive group counts. */
            if (out_plan->stack_count >=
                UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            stack = &out_plan->stacks[out_plan->stack_count++];
            status = copy_text(stack->group_id, sizeof(stack->group_id), group_id);
            /* The new stack is usable only with a complete group identity. */
            if (status != UMI_STATUS_OK) return status;
            stack->placement = placement;
            stack->rect.x = window->x;
            stack->rect.y = window->y;
            stack->rect.width = window->width;
            stack->rect.height = window->height;
        /* Windows in one tab stack must share the same visible geometry. */
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (!rect_matches(window, stack->rect)) {
            return UMI_STATUS_INVALID_STATE;
        }
        /* Refuse another tab before writing beyond the index array. */
        if (stack->window_count >=
            UMI_APPLICATION_SUITE_LAYOUT_STACK_MAX_WINDOWS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        stack->window_indices[stack->window_count++] = index;
        out_plan->visible_window_count += 1U;
        /* Track detached windows for frontend and diagnostic summaries. */
        if (placement == UMI_UI_PLACEMENT_FLOATING)
            out_plan->floating_window_count += 1U;
    }
    /* Empty and all-hidden layouts are deliberate blank canvases. The host
     * can show its add-panel affordance without inventing a docked window. */
    out_plan->source_revision = layout->revision;
    return UMI_STATUS_OK;
}

/* Return one borrowed stack using a bounded index check. */
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_stack_at(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    size_t index)
{
    return plan != NULL &&
        plan->stack_count <= UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS &&
        index < plan->stack_count
        ? &plan->stacks[index] : NULL;
}

/* Find the requested occurrence of one placement in render order. */
const UmiApplicationSuiteLayoutRenderStack *
umi_application_suite_layout_render_plan_find_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement,
    size_t occurrence)
{
    size_t index;
    size_t found = 0U;
    /* Missing plan input has no matching stack. */
    if (plan == NULL ||
        plan->stack_count > UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS) return NULL;
    /* Count only stacks in the requested placement. */
    for (index = 0U; index < plan->stack_count; ++index) {
        /* Other regions do not change the occurrence counter. */
        if (plan->stacks[index].placement != placement) continue;
        /* Return the borrowed stack when the desired occurrence is reached. */
        if (found == occurrence) return &plan->stacks[index];
        found += 1U;
    }
    return NULL;
}

/* Count how many bounded render stacks use one placement. */
size_t umi_application_suite_layout_render_plan_count_placement(
    const UmiApplicationSuiteLayoutRenderPlan *plan,
    UmiUiPlacement placement)
{
    size_t index;
    size_t count = 0U;
    /* Missing plan input contains no render stacks. */
    if (plan == NULL ||
        plan->stack_count > UMI_APPLICATION_SUITE_LAYOUT_MAX_STACKS) return 0U;
    /* Inspect only active stack entries, not unused fixed capacity. */
    for (index = 0U; index < plan->stack_count; ++index)
        /* Each matching stack contributes once to the result. */
        if (plan->stacks[index].placement == placement) count += 1U;
    return count;
}
