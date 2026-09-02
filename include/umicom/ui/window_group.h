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
/**
 * List the named ui window context kind values accepted by this public contract.
 */
typedef enum UmiUiWindowContextKind {
    UMI_UI_WINDOW_CONTEXT_GENERIC = 1,
    UMI_UI_WINDOW_CONTEXT_FILE,
    UMI_UI_WINDOW_CONTEXT_PROJECT,
    UMI_UI_WINDOW_CONTEXT_SYMBOL,
    UMI_UI_WINDOW_CONTEXT_ACCOUNT,
    UMI_UI_WINDOW_CONTEXT_RUN,
    UMI_UI_WINDOW_CONTEXT_DIAGNOSTIC,
    UMI_UI_WINDOW_CONTEXT_INSTRUMENT,
    UMI_UI_WINDOW_CONTEXT_ORDER,
    UMI_UI_WINDOW_CONTEXT_POSITION,
    UMI_UI_WINDOW_CONTEXT_DEAL,
    UMI_UI_WINDOW_CONTEXT_BOOK,
    UMI_UI_WINDOW_CONTEXT_CUSTOMER,
    UMI_UI_WINDOW_CONTEXT_DOCUMENT,
    UMI_UI_WINDOW_CONTEXT_TRACK,
    UMI_UI_WINDOW_CONTEXT_TIMELINE
} UmiUiWindowContextKind;
/**
 * List the named ui window group role values accepted by this public contract.
 */
typedef enum UmiUiWindowGroupRole { UMI_UI_WINDOW_GROUP_SOURCE = 1, UMI_UI_WINDOW_GROUP_DESTINATION, UMI_UI_WINDOW_GROUP_BIDIRECTIONAL } UmiUiWindowGroupRole;
/**
 * Represent the ui window group member data shared with callers of this public contract.
 */
typedef struct UmiUiWindowGroupMember { char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; UmiUiWindowGroupRole role; } UmiUiWindowGroupMember;
/**
 * Represent the ui window group data shared with callers of this public contract.
 */
typedef struct UmiUiWindowGroup { char group_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char colour_token[64U]; UmiUiWindowContextKind context_kind; UmiUiWindowGroupMember members[UMI_UI_WINDOW_GROUP_MAX_MEMBERS]; size_t member_count; uint64_t revision; } UmiUiWindowGroup;
/**
 * Represent the ui window group store data shared with callers of this public contract.
 */
typedef struct UmiUiWindowGroupStore { UmiUiWindowGroup items[UMI_UI_WINDOW_GROUP_MAX]; size_t count; uint64_t revision; } UmiUiWindowGroupStore;
/**
 * Provide the ui window group define operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_group_define(UmiUiWindowGroupStore *store,const char *group_id,const char *colour_token,UmiUiWindowContextKind context_kind);
/**
 * Provide the ui window group join operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_group_join(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id,UmiUiWindowGroupRole role);
/**
 * Provide the ui window group leave operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_group_leave(UmiUiWindowGroupStore *store,const char *group_id,const char *window_id);
/**
 * Copy ui window group into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ui_window_group_assign(
    UmiUiWindowGroupStore *store,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role);
/**
 * Provide the ui window group unassign operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_window_group_unassign(
    UmiUiWindowGroupStore *store,
    const char *window_id);
/**
 * Find ui window group while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiWindowGroup *umi_ui_window_group_find(const UmiUiWindowGroupStore *store,const char *group_id);
/**
 * Provide the ui window group for window operation used by this module and its client
 * applications.
 */
const UmiUiWindowGroup *umi_ui_window_group_for_window(
    const UmiUiWindowGroupStore *store,
    const char *window_id,
    UmiUiWindowGroupRole *out_role);
/**
 * Provide the ui window group route operation used by this module and its client
 * applications.
 */
size_t umi_ui_window_group_route(const UmiUiWindowGroupStore *store,const char *group_id,const char *source_window_id,const char **out_window_ids,size_t capacity);
#endif
