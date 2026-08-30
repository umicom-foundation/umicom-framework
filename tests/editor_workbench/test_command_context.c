/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_command_context.c
 *
 * PURPOSE:
 *   Implement the test command context behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/command_context.h"
int main(void){ UmiEditorWbCommandContext s={true,true,false,true}; if(!umi_editor_wb_command_context_can_edit(&s)||!umi_editor_wb_command_context_can_find(&s))return 1; s.writable=false; if(umi_editor_wb_command_context_can_edit(&s))return 2; return 0; }
