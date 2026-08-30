/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_item_registry.h
 *
 * PURPOSE:
 *   Own a bounded registry of open editor items with stable identities.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_REGISTRY_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_REGISTRY_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/editor/workbench/editor_item.h"
typedef struct UmiEditorWbEditorItemRegistry { UmiEditorWbEditorItem items[UMI_EDITOR_WB_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorWbEditorItemRegistry;
void umi_editor_wb_editor_item_registry_init(UmiEditorWbEditorItemRegistry *registry);
UmiStatus umi_editor_wb_editor_item_registry_upsert(UmiEditorWbEditorItemRegistry *registry,const UmiEditorWbEditorItem *item);
const UmiEditorWbEditorItem *umi_editor_wb_editor_item_registry_find(const UmiEditorWbEditorItemRegistry *registry,const char *id);
UmiStatus umi_editor_wb_editor_item_registry_remove(UmiEditorWbEditorItemRegistry *registry,const char *id);

#ifdef __cplusplus
}
#endif
#endif
