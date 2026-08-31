/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_layout.h
 *
 * PURPOSE:
 *   Publish the public workspace layout contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_LAYOUT_H
#define UMICOM_UI_WORKSPACE_LAYOUT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY 128U
#define UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY 192U
#define UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS 64U
typedef struct UmiUiWorkspaceWindow {
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    double x;
    double y;
    double width;
    double height;
    bool visible;
    bool floating;
    bool maximised;
    bool closable;
    int32_t z_order;
    /* These fields deliberately separate three ideas that older layouts kept
     * in group_id: where a panel is docked, which tab stack contains it and
     * which colour-linked context it follows.  group_id remains the legacy
     * tab-stack value so existing application layouts continue to load. */
    char placement_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char stack_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char context_group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    bool pinned;
    bool resizable;
} UmiUiWorkspaceWindow;
typedef struct UmiUiWorkspaceLayout { char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; UmiUiWorkspaceWindow windows[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS]; size_t window_count; bool locked; uint64_t revision; } UmiUiWorkspaceLayout;
UmiStatus umi_ui_workspace_layout_init(UmiUiWorkspaceLayout *layout,const char *layout_id,const char *name);
UmiStatus umi_ui_workspace_layout_set_locked(UmiUiWorkspaceLayout *layout,bool locked);
UmiStatus umi_ui_workspace_layout_add_window(UmiUiWorkspaceLayout *layout,const UmiUiWorkspaceWindow *window);
UmiStatus umi_ui_workspace_layout_remove_window(UmiUiWorkspaceLayout *layout,const char *window_id);
UmiStatus umi_ui_workspace_layout_place_window(UmiUiWorkspaceLayout *layout,const char *window_id,double x,double y,double width,double height);
UmiStatus umi_ui_workspace_layout_set_maximised(UmiUiWorkspaceLayout *layout,const char *window_id,bool maximised);
UmiStatus umi_ui_workspace_layout_set_visible(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool visible);
UmiStatus umi_ui_workspace_layout_set_floating(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool floating);
UmiStatus umi_ui_workspace_layout_set_group(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *group_id);
UmiStatus umi_ui_workspace_layout_set_placement(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *placement_id);
UmiStatus umi_ui_workspace_layout_set_stack(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *stack_id);
UmiStatus umi_ui_workspace_layout_set_context_group(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    const char *context_group_id);
UmiStatus umi_ui_workspace_layout_set_pinned(
    UmiUiWorkspaceLayout *layout,
    const char *window_id,
    bool pinned);
UmiStatus umi_ui_workspace_layout_rename(
    UmiUiWorkspaceLayout *layout,
    const char *name);
const UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window(const UmiUiWorkspaceLayout *layout,const char *window_id);
UmiUiWorkspaceWindow *umi_ui_workspace_layout_find_window_mutable(
    UmiUiWorkspaceLayout *layout,
    const char *window_id);
size_t umi_ui_workspace_layout_count_tool(
    const UmiUiWorkspaceLayout *layout,
    const char *tool_id);
UmiStatus umi_ui_workspace_layout_validate(const UmiUiWorkspaceLayout *layout,char *out_reason,size_t capacity);
UmiStatus umi_ui_workspace_layout_clone(const UmiUiWorkspaceLayout *source,const char *layout_id,const char *name,UmiUiWorkspaceLayout *out_layout);
#endif
