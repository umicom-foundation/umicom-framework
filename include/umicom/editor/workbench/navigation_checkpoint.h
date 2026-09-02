/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/navigation_checkpoint.h
 *
 * PURPOSE:
 *   Capture a named source-location checkpoint and its navigation reason.
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
#ifndef UMICOM_EDITOR_WORKBENCH_NAVIGATION_CHECKPOINT_H
#define UMICOM_EDITOR_WORKBENCH_NAVIGATION_CHECKPOINT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb navigation checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbNavigationCheckpoint { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbNavigationCheckpoint;
/**
 * Initialise editor wb navigation checkpoint from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_navigation_checkpoint_init(UmiEditorWbNavigationCheckpoint *state,const char *id,const char *text); UmiStatus umi_editor_wb_navigation_checkpoint_set_values(UmiEditorWbNavigationCheckpoint *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_navigation_checkpoint_valid(const UmiEditorWbNavigationCheckpoint *state);

#ifdef __cplusplus
}
#endif
#endif
