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
/**
 * Represent the editor wb editor item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorItemRegistry { UmiEditorWbEditorItem items[UMI_EDITOR_WB_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorWbEditorItemRegistry;
/**
 * Initialise editor wb editor item registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_item_registry_init(UmiEditorWbEditorItemRegistry *registry);
/**
 * Provide the editor wb editor item registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_editor_item_registry_upsert(UmiEditorWbEditorItemRegistry *registry,const UmiEditorWbEditorItem *item);
/**
 * Find editor wb editor item registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorWbEditorItem *umi_editor_wb_editor_item_registry_find(const UmiEditorWbEditorItemRegistry *registry,const char *id);
/**
 * Remove editor wb editor item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_editor_item_registry_remove(UmiEditorWbEditorItemRegistry *registry,const char *id);

#ifdef __cplusplus
}
#endif
#endif
