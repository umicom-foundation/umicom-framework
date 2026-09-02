/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/recently_closed_editor.h
 *
 * PURPOSE:
 *   Maintain a bounded LIFO history of recently closed resources.
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
#ifndef UMICOM_EDITOR_WORKBENCH_RECENTLY_CLOSED_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_RECENTLY_CLOSED_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb recently closed editor data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbRecentlyClosedEditor { char paths[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_PATH_CAPACITY]; size_t count; } UmiEditorWbRecentlyClosedEditor;
/**
 * Initialise editor wb recently closed editor from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_recently_closed_editor_init(UmiEditorWbRecentlyClosedEditor *history);
/**
 * Provide the editor wb recently closed editor push operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_recently_closed_editor_push(UmiEditorWbRecentlyClosedEditor *history,const char *path);
/**
 * Provide the editor wb recently closed editor pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_recently_closed_editor_pop(UmiEditorWbRecentlyClosedEditor *history,char *out_path,size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
