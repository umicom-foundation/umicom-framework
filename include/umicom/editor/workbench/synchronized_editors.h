/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/synchronized_editors.h
 *
 * PURPOSE:
 *   Maintain a bounded set of editor surfaces participating in synchronization.
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
#ifndef UMICOM_EDITOR_WORKBENCH_SYNCHRONIZED_EDITORS_H
#define UMICOM_EDITOR_WORKBENCH_SYNCHRONIZED_EDITORS_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb synchronized editors data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbSynchronizedEditors { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbSynchronizedEditors;
/**
 * Initialise editor wb synchronized editors from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_synchronized_editors_init(UmiEditorWbSynchronizedEditors *state);
/**
 * Add editor wb synchronized editors only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_wb_synchronized_editors_append(UmiEditorWbSynchronizedEditors *state,const char *text,uint32_t depth);
/**
 * Provide the editor wb synchronized editors activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_wb_synchronized_editors_activate(UmiEditorWbSynchronizedEditors *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
