/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/outline_panel.h
 *
 * PURPOSE:
 *   Compose a flattened document-outline panel from symbol entries and depth.
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
#ifndef UMICOM_EDITOR_WORKBENCH_OUTLINE_PANEL_H
#define UMICOM_EDITOR_WORKBENCH_OUTLINE_PANEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb outline panel data shared with callers of this public contract.
 */
typedef struct UmiEditorWbOutlinePanel { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbOutlinePanel;
/**
 * Initialise editor wb outline panel from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_outline_panel_init(UmiEditorWbOutlinePanel *state);
/**
 * Add editor wb outline panel only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_outline_panel_append(UmiEditorWbOutlinePanel *state,const char *text,uint32_t depth);
/**
 * Provide the editor wb outline panel activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_outline_panel_activate(UmiEditorWbOutlinePanel *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
