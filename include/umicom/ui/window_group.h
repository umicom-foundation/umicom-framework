/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/window_group.h
 *
 * PURPOSE:
 *   Publish the public window group contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WINDOW_GROUP_H
#define UMICOM_UI_WINDOW_GROUP_H
#include "umicom/ui/workspace_layout.h"
#define UMI_UI_WINDOW_GROUP_MAX 16U
#define UMI_UI_WINDOW_GROUP_MAX_MEMBERS 32U
typedef enum UmiUiWindowContextKind { UMI_UI_WINDOW_CONTEXT_GENERIC = 1, UMI_UI_WINDOW_CONTEXT_FILE, UMI_UI_WINDOW_CONTEXT_PROJECT, UMI_UI_WINDOW_CONTEXT_SYMBOL, UMI_UI_WINDOW_CONTEXT_ACCOUNT, UMI_UI_WINDOW_CONTEXT_RUN, UMI_UI_WINDOW_CONTEXT_DIAGNOSTIC } UmiUiWindowContextKind;
typedef enum UmiUiWindowGroupRole { UMI_UI_WINDOW_GROUP_SOURCE = 1, UMI_UI_WINDOW_GROUP_DESTINATION, UMI_UI_WINDOW_GROUP_BIDIRECTIONAL } UmiUiWindowGroupRole;
typedef struct UmiUiWindowGroupMember { char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowGroupRole role; } UmiUiWindowGroupMember;
typedef struct UmiUiWindowGroup { char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char colour_token[64U]; UmiUiWindowContextKind context_kind; UmiUiWindowGroupMember members[UMI_UI_WINDOW_GROUP_MAX_MEMBERS]; size_t member_count; uint64_t revision; } UmiUiWindowGroup;
typedef struct UmiUiWindowGroupStore { UmiUiWindowGroup items[UMI_UI_WINDOW_GROUP_MAX]; size_t count; uint64_t revision; } UmiUiWindowGroupStore;
UmiStatus umi_ui_window_group_define(UmiUiWindowGroupStore *store,const char *group_id,const char *colour_token,UmiUiWindowContextKind context_kind);
UmiStatus umi_ui_window_group_join(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id,UmiUiWindowGroupRole role);
UmiStatus umi_ui_window_group_leave(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id);
const UmiUiWindowGroup *umi_ui_window_group_find(const UmiUiWindowGroupStore *store,const char *group_id);
size_t umi_ui_window_group_route(const UmiUiWindowGroupStore *store,const char *group_id,const char *source_window_id,const char **out_window_ids,size_t capacity);
#endif
