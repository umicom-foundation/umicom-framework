/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_line_number_model.c
 *
 * PURPOSE:
 *   Implement the test line number model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/line_number_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbLineNumberModel m; umi_editor_wb_line_number_model_init(&m,UMI_EDITOR_WB_LINE_NUMBERS_RELATIVE,10U); /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_line_number_model_value(&m,7U)!=3U)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_line_number_model_value(&m,10U)!=10U)return 2; return 0; }
