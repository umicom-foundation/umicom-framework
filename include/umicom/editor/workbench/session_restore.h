/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/session_restore.h
 *
 * PURPOSE:
 *   Build a bounded editor-session restore plan including skipped missing resources.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SESSION_RESTORE_H
#define UMICOM_EDITOR_WORKBENCH_SESSION_RESTORE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbSessionRestore { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbSessionRestore;
UmiStatus umi_editor_wb_session_restore_init(UmiEditorWbSessionRestore *state,const char *id,const char *text); UmiStatus umi_editor_wb_session_restore_set_values(UmiEditorWbSessionRestore *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_session_restore_valid(const UmiEditorWbSessionRestore *state);

#ifdef __cplusplus
}
#endif
#endif
