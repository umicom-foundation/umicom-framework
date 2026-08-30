/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/workspace_profile.h
 *
 * PURPOSE:
 *   Describe reusable editor-workspace defaults without moving them into Studio.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_WORKSPACE_PROFILE_H
#define UMICOM_EDITOR_WORKBENCH_WORKSPACE_PROFILE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbWorkspaceProfile { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbWorkspaceProfile;
UmiStatus umi_editor_wb_workspace_profile_init(UmiEditorWbWorkspaceProfile *state,const char *id,const char *text); UmiStatus umi_editor_wb_workspace_profile_set_values(UmiEditorWbWorkspaceProfile *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_workspace_profile_valid(const UmiEditorWbWorkspaceProfile *state);

#ifdef __cplusplus
}
#endif
#endif
