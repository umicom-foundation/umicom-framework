/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/bookmark_panel.h
 *
 * PURPOSE:
 *   Maintain a reusable bookmark-panel model over stable source locations.
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
#ifndef UMICOM_EDITOR_WORKBENCH_BOOKMARK_PANEL_H
#define UMICOM_EDITOR_WORKBENCH_BOOKMARK_PANEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb bookmark panel data shared with callers of this public contract.
 */
typedef struct UmiEditorWbBookmarkPanel { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbBookmarkPanel;
/**
 * Initialise editor wb bookmark panel from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_bookmark_panel_init(UmiEditorWbBookmarkPanel *state);
/**
 * Add editor wb bookmark panel only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_bookmark_panel_add(UmiEditorWbBookmarkPanel *state,const char *id);
/**
 * Provide the editor wb bookmark panel contains operation used by this module and its
 * client applications.
 */
int umi_editor_wb_bookmark_panel_contains(const UmiEditorWbBookmarkPanel *state,const char *id);
/**
 * Remove editor wb bookmark panel while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_bookmark_panel_remove(UmiEditorWbBookmarkPanel *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
