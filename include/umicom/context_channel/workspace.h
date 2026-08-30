/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/workspace.h
 *
 * PURPOSE:
 *   Define the canonical workspace context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_WORKSPACE_H
#define UMICOM_CONTEXT_CHANNEL_WORKSPACE_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkspaceContext {
    uint32_t structure_size;
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char profile_id[UMI_CONTEXT_TEXT_CAPACITY];
    char root_path[UMI_CONTEXT_PATH_CAPACITY];
    char perspective_id[UMI_CONTEXT_TEXT_CAPACITY];
    char layout_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool trusted;
    uint64_t revision;
} UmiWorkspaceContext;
void umi_workspace_context_init(UmiWorkspaceContext *context);
UmiStatus umi_workspace_context_validate(const UmiWorkspaceContext *context);
UmiStatus umi_workspace_context_copy(UmiWorkspaceContext *destination, const UmiWorkspaceContext *source);
UmiStatus umi_workspace_context_set_workspace_id(UmiWorkspaceContext *context, const char *value);
UmiStatus umi_workspace_context_set_profile_id(UmiWorkspaceContext *context, const char *value);
UmiStatus umi_workspace_context_set_root_path(UmiWorkspaceContext *context, const char *value);
UmiStatus umi_workspace_context_set_perspective_id(UmiWorkspaceContext *context, const char *value);
UmiStatus umi_workspace_context_set_layout_id(UmiWorkspaceContext *context, const char *value);
UmiStatus umi_workspace_context_set_trusted(UmiWorkspaceContext *context, bool value);
#ifdef __cplusplus
}
#endif
#endif
