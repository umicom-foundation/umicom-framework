/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/compare_editor.h
 *
 * PURPOSE:
 *   Describe a side-by-side editor comparison and synchronized navigation policy.
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
#ifndef UMICOM_EDITOR_WORKBENCH_COMPARE_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_COMPARE_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb compare editor data shared with callers of this public contract.
 */
typedef struct UmiEditorWbCompareEditor { char left_id[UMI_EDITOR_WB_ID_CAPACITY]; char right_id[UMI_EDITOR_WB_ID_CAPACITY]; bool sync_scroll; bool ignore_whitespace; } UmiEditorWbCompareEditor;
/**
 * Initialise editor wb compare editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_compare_editor_init(UmiEditorWbCompareEditor *state,const char *left_id,const char *right_id);

#ifdef __cplusplus
}
#endif
#endif
