/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_replace_session.h
 *
 * PURPOSE:
 *   Track in-editor replacement text and applied replacement count.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_REPLACE_SESSION_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_REPLACE_SESSION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorReplaceSession { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbEditorReplaceSession;
UmiStatus umi_editor_wb_editor_replace_session_init(UmiEditorWbEditorReplaceSession *state,const char *id,const char *text); UmiStatus umi_editor_wb_editor_replace_session_set_values(UmiEditorWbEditorReplaceSession *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_editor_replace_session_valid(const UmiEditorWbEditorReplaceSession *state);

#ifdef __cplusplus
}
#endif
#endif
