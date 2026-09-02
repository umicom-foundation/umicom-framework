/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_group_registry.h
 *
 * PURPOSE:
 *   Own a bounded registry of editor groups.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_GROUP_REGISTRY_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_GROUP_REGISTRY_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor group registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorGroupRegistry { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbEditorGroupRegistry;
/**
 * Initialise editor wb editor group registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_group_registry_init(UmiEditorWbEditorGroupRegistry *state);
/**
 * Add editor wb editor group registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_wb_editor_group_registry_add(UmiEditorWbEditorGroupRegistry *state,const char *id);
/**
 * Provide the editor wb editor group registry contains operation used by this module and
 * its client applications.
 */
int umi_editor_wb_editor_group_registry_contains(const UmiEditorWbEditorGroupRegistry *state,const char *id);
/**
 * Remove editor wb editor group registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_wb_editor_group_registry_remove(UmiEditorWbEditorGroupRegistry *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
