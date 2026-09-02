/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/command_set.h
 *
 * PURPOSE:
 *   Maintain a bounded editor command enablement table.
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
#ifndef UMICOM_EDITOR_WORKBENCH_COMMAND_SET_H
#define UMICOM_EDITOR_WORKBENCH_COMMAND_SET_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb command set data shared with callers of this public contract.
 */
typedef struct UmiEditorWbCommandSet { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbCommandSet;
/**
 * Initialise editor wb command set from caller-provided values so later operations receive
 * a known state.
 */
void umi_editor_wb_command_set_init(UmiEditorWbCommandSet *state);
/**
 * Add editor wb command set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_command_set_add(UmiEditorWbCommandSet *state,const char *id);
/**
 * Provide the editor wb command set contains operation used by this module and its client
 * applications.
 */
int umi_editor_wb_command_set_contains(const UmiEditorWbCommandSet *state,const char *id);
/**
 * Remove editor wb command set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_command_set_remove(UmiEditorWbCommandSet *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
