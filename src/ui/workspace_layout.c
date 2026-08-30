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

static UmiUiWorkspaceWindow *find_mutable(UmiUiWorkspaceLayout *layout,const char *window_id)
{ size_t index; if (layout == NULL || window_id == NULL) return NULL; for (index = 0U; index < layout->window_count; ++index) if (strcmp(layout->windows[index].window_id,window_id) == 0) return &layout->windows[index]; return NULL; }
static bool rectangle_valid(double x,double y,double width,double height) { return x >= 0.0 && y >= 0.0 && width > 0.0 && height > 0.0 && x + width <= 1.000001 && y + height <= 1.000001; }
UmiStatus umi_ui_workspace_layout_init(UmiUiWorkspaceLayout *layout,const char *layout_id,const char *name)
{
    int first; int second;
    if (layout == NULL || layout_id == NULL || name == NULL || layout_id[0] == '\0' || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(layout,0,sizeof(*layout));
    first = snprintf(layout->layout_id,sizeof(layout->layout_id),"%s",layout_id); second = snprintf(layout->name,sizeof(layout->name),"%s",name);
    if (first < 0 || second < 0 || (size_t)first >= sizeof(layout->layout_id) || (size_t)second >= sizeof(layout->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->locked = true; layout->revision = 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_set_locked(UmiUiWorkspaceLayout *layout,bool locked) { char reason[192U]; if (layout == NULL) return UMI_STATUS_INVALID_ARGUMENT; if (locked && umi_ui_workspace_layout_validate(layout,reason,sizeof(reason)) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE; layout->locked = locked; layout->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_ui_workspace_layout_add_window(UmiUiWorkspaceLayout *layout,const UmiUiWorkspaceWindow *window)
{
    if (layout == NULL || window == NULL || window->window_id[0] == '\0' || window->tool_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    if (!rectangle_valid(window->x,window->y,window->width,window->height)) return UMI_STATUS_INVALID_ARGUMENT;
    if (find_mutable(layout,window->window_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (layout->window_count >= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->windows[layout->window_count++] = *window; layout->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_remove_window(UmiUiWorkspaceLayout *layout,const char *window_id)
{
    size_t index;
    if (layout == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    for (index = 0U; index < layout->window_count; ++index) if (strcmp(layout->windows[index].window_id,window_id) == 0) { if (!layout->windows[index].closable) return UMI_STATUS_PERMISSION_DENIED; (void)memmove(&layout->windows[index],&layout->windows[index + 1U],(layout->window_count - index - 1U) * sizeof(layout->windows[0])); layout->window_count -= 1U; layout->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_ui_workspace_layout_place_window(UmiUiWorkspaceLayout *layout,const char *window_id,double x,double y,double width,double height)
{
    UmiUiWorkspaceWindow *window;
    if (layout == NULL || layout->locked || !rectangle_valid(x,y,width,height)) return layout != NULL && layout->locked ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_INVALID_ARGUMENT;
    window = find_mutable(layout,window_id); if (window == NULL) return UMI_STATUS_NOT_FOUND;
    window->x = x; window->y = y; window->width = width; window->height = height; layout->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_set_maximised(UmiUiWorkspaceLayout *layout,const char *window_id,bool maximised) { UmiUiWorkspaceWindow *window = find_mutable(layout,window_id); if (window == NULL) return layout == NULL || window_id == NULL ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_NOT_FOUND; window->maximised = maximised; layout->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_ui_workspace_layout_set_visible(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool visible)
{
    UmiUiWorkspaceWindow *window;
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (window->visible != visible) {
        window->visible = visible;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_set_floating(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool floating)
{
    UmiUiWorkspaceWindow *window;
    if (layout == NULL || window_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    if (window->floating != floating) {
        window->floating = floating;
        layout->revision += 1U;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_set_group(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *group_id)
{
    UmiUiWorkspaceWindow *window;
    int written;
    if (layout == NULL || window_id == NULL || group_id == NULL ||
        group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = find_mutable(layout, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(window->group_id, sizeof(window->group_id), "%s", group_id);
    if (written < 0 || (size_t)written >= sizeof(window->group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_rename(
    UmiUiWorkspaceLayout *layout,
    const char *name)
{
    int written;
    if (layout == NULL || name == NULL || name[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    written = snprintf(layout->name, sizeof(layout->name), "%s", name);
    if (written < 0 || (size_t)written >= sizeof(layout->name))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window(const UmiUiWorkspaceLayout *layout,const char *window_id) { return find_mutable((UmiUiWorkspaceLayout *)(void *)layout,window_id); }
UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window_mutable(
    UmiUiWorkspaceLayout *layout,
    const char *window_id)
{
    return find_mutable(layout, window_id);
}
size_t umi_ui_workspace_layout_count_tool(
    const UmiUiWorkspaceLayout *layout,
    const char *tool_id)
{
    size_t index;
    size_t count = 0U;
    if (layout == NULL || tool_id == NULL) return 0U;
    for (index = 0U; index < layout->window_count; ++index) {
        if (strcmp(layout->windows[index].tool_id, tool_id) == 0) count += 1U;
    }
    return count;
}
UmiStatus umi_ui_workspace_layout_validate(const UmiUiWorkspaceLayout *layout,char *out_reason,size_t capacity)
{
    size_t index; int length;
    if (layout == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < layout->window_count; ++index) if (!rectangle_valid(layout->windows[index].x,layout->windows[index].y,layout->windows[index].width,layout->windows[index].height)) { length = snprintf(out_reason,capacity,"Window %s is outside the normalised workspace",layout->windows[index].window_id); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Layout is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
UmiStatus umi_ui_workspace_layout_clone(const UmiUiWorkspaceLayout *source,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout)
{
    int first; int second;
    if (source == NULL || layout_id == NULL || name == NULL || out_layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_layout = *source; first = snprintf(out_layout->layout_id,sizeof(out_layout->layout_id),"%s",layout_id); second = snprintf(out_layout->name,sizeof(out_layout->name),"%s",name);
    if (first < 0 || second < 0 || (size_t)first >= sizeof(out_layout->layout_id) || (size_t)second >= sizeof(out_layout->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    out_layout->revision = 1U; return UMI_STATUS_OK;
}
