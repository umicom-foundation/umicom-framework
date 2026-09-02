/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/navigation_restore.h
 *
 * PURPOSE:
 *   Represent a validated restore plan for a prior editor location.
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
#ifndef UMICOM_EDITOR_WORKBENCH_NAVIGATION_RESTORE_H
#define UMICOM_EDITOR_WORKBENCH_NAVIGATION_RESTORE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb navigation restore data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbNavigationRestore { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbNavigationRestore;
/**
 * Initialise editor wb navigation restore from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_navigation_restore_init(UmiEditorWbNavigationRestore *state,const char *id,const char *text); UmiStatus umi_editor_wb_navigation_restore_set_values(UmiEditorWbNavigationRestore *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_navigation_restore_valid(const UmiEditorWbNavigationRestore *state);

#ifdef __cplusplus
}
#endif
#endif
