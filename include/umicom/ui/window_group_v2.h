/* Umicom Framework | Linked workspace windows v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_WINDOW_GROUP_V2_H
#define UMICOM_UI_WINDOW_GROUP_V2_H
#include "umicom/ui/workspace_layout_v3.h"
#define UMI_UI_WINDOW_GROUP_MAX 16U
#define UMI_UI_WINDOW_GROUP_MAX_MEMBERS 32U
typedef enum UmiUiWindowContextKind { UMI_UI_WINDOW_CONTEXT_GENERIC = 1, UMI_UI_WINDOW_CONTEXT_FILE, UMI_UI_WINDOW_CONTEXT_PROJECT, UMI_UI_WINDOW_CONTEXT_SYMBOL, UMI_UI_WINDOW_CONTEXT_ACCOUNT, UMI_UI_WINDOW_CONTEXT_RUN, UMI_UI_WINDOW_CONTEXT_DIAGNOSTIC } UmiUiWindowContextKind;
typedef enum UmiUiWindowGroupRole { UMI_UI_WINDOW_GROUP_SOURCE = 1, UMI_UI_WINDOW_GROUP_DESTINATION, UMI_UI_WINDOW_GROUP_BIDIRECTIONAL } UmiUiWindowGroupRole;
typedef struct UmiUiWindowGroupMember { char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowGroupRole role; } UmiUiWindowGroupMember;
typedef struct UmiUiWindowGroupV2 { char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char colour_token[64U]; UmiUiWindowContextKind context_kind; UmiUiWindowGroupMember members[UMI_UI_WINDOW_GROUP_MAX_MEMBERS]; size_t member_count; uint64_t revision; } UmiUiWindowGroupV2;
typedef struct UmiUiWindowGroupStoreV2 { UmiUiWindowGroupV2 items[UMI_UI_WINDOW_GROUP_MAX]; size_t count; uint64_t revision; } UmiUiWindowGroupStoreV2;
UmiStatus umi_ui_window_group_v2_define(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *colour_token,UmiUiWindowContextKind context_kind);
UmiStatus umi_ui_window_group_v2_join(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *window_id,UmiUiWindowGroupRole role);
UmiStatus umi_ui_window_group_v2_leave(UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *window_id);
const UmiUiWindowGroupV2 *umi_ui_window_group_v2_find(const UmiUiWindowGroupStoreV2 *store,const char *group_id);
size_t umi_ui_window_group_v2_route(const UmiUiWindowGroupStoreV2 *store,const char *group_id,const char *source_window_id,const char **out_window_ids,size_t capacity);
#endif
