/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/command_context.c
 *
 * PURPOSE:
 *   Describe current editor command context such as writability and selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/command_context.h"

int umi_editor_wb_command_context_can_edit(const UmiEditorWbCommandContext *s){return s!=NULL&&s->has_document&&s->writable;} int umi_editor_wb_command_context_can_find(const UmiEditorWbCommandContext *s){return s!=NULL&&s->has_document;}
