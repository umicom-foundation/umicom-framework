/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/keymap_context.h
 *
 * PURPOSE:
 *   Represent a key chord and semantic editor context constraint.
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
#ifndef UMICOM_EDITOR_WORKBENCH_KEYMAP_CONTEXT_H
#define UMICOM_EDITOR_WORKBENCH_KEYMAP_CONTEXT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbKeymapContext { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbKeymapContext;
UmiStatus umi_editor_wb_keymap_context_init(UmiEditorWbKeymapContext *state,const char *id,const char *text); UmiStatus umi_editor_wb_keymap_context_set_values(UmiEditorWbKeymapContext *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_keymap_context_valid(const UmiEditorWbKeymapContext *state);

#ifdef __cplusplus
}
#endif
#endif
