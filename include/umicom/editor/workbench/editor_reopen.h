/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_reopen.h
 *
 * PURPOSE:
 *   Build a reopen request from recently-closed editor evidence.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_REOPEN_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_REOPEN_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorReopen { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorReopen;
UmiStatus umi_editor_wb_editor_reopen_init(UmiEditorWbEditorReopen *state,const char *resource,const char *group_id);
int umi_editor_wb_editor_reopen_valid(const UmiEditorWbEditorReopen *state);

#ifdef __cplusplus
}
#endif
#endif
