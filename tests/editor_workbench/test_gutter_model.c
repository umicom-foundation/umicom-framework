/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_gutter_model.c
 *
 * PURPOSE:
 *   Implement the test gutter model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/gutter_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbGutterModel s; umi_editor_wb_gutter_model_init(&s); /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_gutter_model_channel_count(&s)!=4U)return 1; return 0; }
