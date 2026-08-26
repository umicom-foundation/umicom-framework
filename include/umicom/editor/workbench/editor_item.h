/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_item.h
 *
 * PURPOSE:
 *   Describe one open editor item independently of any toolkit tab widget.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorItem { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; char path[UMI_EDITOR_WB_PATH_CAPACITY]; UmiEditorWbOpenMode open_mode; bool dirty; bool pinned; uint64_t revision; } UmiEditorWbEditorItem;
UmiStatus umi_editor_wb_editor_item_init(UmiEditorWbEditorItem *item,const char *id,const char *path,UmiEditorWbOpenMode mode);
UmiStatus umi_editor_wb_editor_item_set_dirty(UmiEditorWbEditorItem *item,bool dirty);
int umi_editor_wb_editor_item_valid(const UmiEditorWbEditorItem *item);

#ifdef __cplusplus
}
#endif
#endif
