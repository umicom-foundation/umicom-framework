/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_search_session.h
 *
 * PURPOSE:
 *   Track in-editor search query, options and active-result state.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SEARCH_SESSION_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SEARCH_SESSION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorSearchSession { char query[UMI_EDITOR_WB_TEXT_CAPACITY]; bool case_sensitive; bool whole_word; bool regex; size_t match_count; size_t active_index; } UmiEditorWbEditorSearchSession;
UmiStatus umi_editor_wb_editor_search_session_init(UmiEditorWbEditorSearchSession *session,const char *query);
UmiStatus umi_editor_wb_editor_search_session_set_matches(UmiEditorWbEditorSearchSession *session,size_t match_count);
size_t umi_editor_wb_editor_search_session_next(UmiEditorWbEditorSearchSession *session,int direction);

#ifdef __cplusplus
}
#endif
#endif
