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

typedef struct UmiEditorWbEditorGroupRegistry { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbEditorGroupRegistry;
void umi_editor_wb_editor_group_registry_init(UmiEditorWbEditorGroupRegistry *state);
UmiStatus umi_editor_wb_editor_group_registry_add(UmiEditorWbEditorGroupRegistry *state,const char *id);
int umi_editor_wb_editor_group_registry_contains(const UmiEditorWbEditorGroupRegistry *state,const char *id);
UmiStatus umi_editor_wb_editor_group_registry_remove(UmiEditorWbEditorGroupRegistry *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
