/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_layout.c
 *
 * PURPOSE:
 *   Implement the workspace layout behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_layout.h"
#include <stdio.h>
#include <string.h>

/* Provide the find mutable operation used by this module and its client applications. */
static UmiUiWorkspaceWindow *find_mutable(UmiUiWorkspaceLayout *layout,const char *window_id)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (layout == NULL || window_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < layout->window_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(layout->windows[index].window_id,window_id) == 0) return &layout->windows[index]; return NULL; }
/* Check that rectangle satisfies its contract before another service relies on it. */
static bool rectangle_valid(double x,double y,double width,double height) { return x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 && x + width <= 1.000001 && y + height <= 1.000001; }
/*
 * Provide the copy optional text operation used by this module and its client
 * applications.
 */
static UmiStatus copy_optional_text(char *destination,size_t capacity,const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination,capacity,"%s",source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/*
 * Initialise ui workspace layout from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_workspace_layout_init(UmiUiWorkspaceLayout *layout,const char *layout_id,const char *name)
{
    int first; int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || layout_id == NULL || name == NULL || layout_id[0] == '\0' || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(layout,0,sizeof(*layout));
    first = snprintf(layout->layout_id,sizeof(layout->layout_id),"%s",layout_id); second = snprintf(layout->name,sizeof(layout->name),"%s",name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first < 0 || second < 0 || (size_t)first >= sizeof(layout->layout_id) || (size_t)second >= sizeof(layout->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->locked = true; layout->revision = 1U; return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set locked operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_set_locked(UmiUiWorkspaceLayout *layout,bool locked) { char reason[192U]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (layout == NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (locked && umi_ui_workspace_layout_validate(layout,reason,sizeof(reason)) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE; layout->locked = locked; layout->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the ui workspace layout add window operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_add_window(UmiUiWorkspaceLayout *layout,const UmiUiWorkspaceWindow *window)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window == NULL || window->window_id[0] == '\0' || window->tool_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!rectangle_valid(window->x,window->y,window->width,window->height)) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_mutable(layout,window->window_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (layout->window_count >= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->windows[layout->window_count++] = *window; layout->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout remove window operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_remove_window(UmiUiWorkspaceLayout *layout,const char *window_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(layout->windows[index].window_id,window_id) == 0) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!layout->windows[index].closable) return UMI_STATUS_PERMISSION_DENIED; (void)memmove(&layout->windows[index],&layout->windows[index + 1U],(layout->window_count - index - 1U) * sizeof(layout->windows[0])); layout->window_count -= 1U; layout->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Provide the ui workspace layout place window operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_place_window(UmiUiWorkspaceLayout *layout,const char *window_id,double x,double y,double width,double height)
{
    UmiUiWorkspaceWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || layout->locked || !rectangle_valid(x,y,width,height)) return layout != NULL && layout->locked ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_INVALID_ARGUMENT;
    window = find_mutable(layout,window_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (window == NULL) return UMI_STATUS_NOT_FOUND;
    window->x = x; window->y = y; window->width = width; window->height = height; layout->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set maximised operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_set_maximised(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool maximised)
{
    UmiUiWorkspaceWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->maximised != maximised) {
        window->maximised = maximised;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set visible operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_set_visible(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool visible)
{
    UmiUiWorkspaceWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this operation only while the related capability or state is available. */
    if (window->visible != visible) {
        window->visible = visible;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set floating operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_set_floating(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool floating)
{
    UmiUiWorkspaceWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->floating != floating) {
        window->floating = floating;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set group operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_set_group(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *group_id)
{
    UmiUiWorkspaceWindow *window;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL || group_id == NULL ||
        group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(window->group_id, sizeof(window->group_id), "%s", group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(window->group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the explicit stack in step with the legacy field.  New code reads
     * stack_id; old saved layouts and callers can continue using group_id. */
    if (copy_optional_text(window->stack_id, sizeof(window->stack_id),
                           group_id) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout set placement operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_set_placement(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *placement_id)
{
    UmiUiWorkspaceWindow *window;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL || placement_id == NULL ||
        placement_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_optional_text(window->placement_id,
                                sizeof(window->placement_id), placement_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) layout->revision += 1U;
    return status;
}
/*
 * Provide the ui workspace layout set stack operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_set_stack(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *stack_id)
{
    UmiUiWorkspaceWindow *window;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL || stack_id == NULL ||
        stack_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_optional_text(window->stack_id, sizeof(window->stack_id),
                                stack_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_optional_text(window->group_id, sizeof(window->group_id),
                                    stack_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) layout->revision += 1U;
    return status;
}
/*
 * Provide the ui workspace layout set context group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workspace_layout_set_context_group(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *context_group_id)
{
    UmiUiWorkspaceWindow *window;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL || context_group_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_optional_text(window->context_group_id,
                                sizeof(window->context_group_id),
                                context_group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) layout->revision += 1U;
    return status;
}
/*
 * Provide the ui workspace layout set pinned operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_set_pinned(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool pinned)
{
    UmiUiWorkspaceWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->pinned != pinned) {
        window->pinned = pinned;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout rename operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_rename(
    UmiUiWorkspaceLayout *layout,
    const char *name)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || name == NULL || name[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    written = snprintf(layout->name, sizeof(layout->name), "%s", name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(layout->name))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout find window operation used by this module and its client
 * applications.
 */
const UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window(const UmiUiWorkspaceLayout *layout,const char *window_id) { return find_mutable((UmiUiWorkspaceLayout *)(void *)layout,window_id); }
/*
 * Provide the ui workspace layout find window mutable operation used by this module and
 * its client applications.
 */
UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window_mutable(
    UmiUiWorkspaceLayout *layout,
    const char *window_id)
{
    return find_mutable(layout, window_id);
}
/*
 * Provide the ui workspace layout count tool operation used by this module and its client
 * applications.
 */
size_t umi_ui_workspace_layout_count_tool(
    const UmiUiWorkspaceLayout *layout,
    const char *tool_id)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || tool_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(layout->windows[index].tool_id, tool_id) == 0) count += 1U;
    }
    return count;
}
/*
 * Check that ui workspace layout satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ui_workspace_layout_validate(const UmiUiWorkspaceLayout *layout,char *out_reason,size_t capacity)
{
    size_t index; int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!rectangle_valid(layout->windows[index].x,layout->windows[index].y,layout->windows[index].width,layout->windows[index].height)) { length = snprintf(out_reason,capacity,"Window %s is outside the normalised workspace",layout->windows[index].window_id); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Layout is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/*
 * Provide the ui workspace layout clone operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workspace_layout_clone(const UmiUiWorkspaceLayout *source,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout)
{
    int first; int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || layout_id == NULL || name == NULL || out_layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_layout = *source; first = snprintf(out_layout->layout_id,sizeof(out_layout->layout_id),"%s",layout_id); second = snprintf(out_layout->name,sizeof(out_layout->name),"%s",name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first < 0 || second < 0 || (size_t)first >= sizeof(out_layout->layout_id) || (size_t)second >= sizeof(out_layout->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    out_layout->revision = 1U; return UMI_STATUS_OK;
}
