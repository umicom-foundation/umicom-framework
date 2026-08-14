/*-----------------------------------------------------------------------------
 * Umicom Framework | Professional workspace layout v3
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_LAYOUT_V3_H
#define UMICOM_UI_WORKSPACE_LAYOUT_V3_H
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
} UmiUiWorkspaceWindow;
typedef struct UmiUiWorkspaceLayoutV3 { char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; UmiUiWorkspaceWindow windows[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS]; size_t window_count; bool locked; uint64_t revision; } UmiUiWorkspaceLayoutV3;
UmiStatus umi_ui_workspace_layout_v3_init(UmiUiWorkspaceLayoutV3 *layout,const char *layout_id,const char *name);
UmiStatus umi_ui_workspace_layout_v3_set_locked(UmiUiWorkspaceLayoutV3 *layout,bool locked);
UmiStatus umi_ui_workspace_layout_v3_add_window(UmiUiWorkspaceLayoutV3 *layout,const UmiUiWorkspaceWindow *window);
UmiStatus umi_ui_workspace_layout_v3_remove_window(UmiUiWorkspaceLayoutV3 *layout,const char *window_id);
UmiStatus umi_ui_workspace_layout_v3_place_window(UmiUiWorkspaceLayoutV3 *layout,const char *window_id,double x,double y,double width,double height);
UmiStatus umi_ui_workspace_layout_v3_set_maximised(UmiUiWorkspaceLayoutV3 *layout,const char *window_id,bool maximised);
const UmiUiWorkspaceWindow *umi_ui_workspace_layout_v3_find_window(const UmiUiWorkspaceLayoutV3 *layout,const char *window_id);
UmiStatus umi_ui_workspace_layout_v3_validate(const UmiUiWorkspaceLayoutV3 *layout,char *out_reason,size_t capacity);
UmiStatus umi_ui_workspace_layout_v3_clone(const UmiUiWorkspaceLayoutV3 *source,const char *layout_id,const char *name,UmiUiWorkspaceLayoutV3 *out_layout);
#endif
