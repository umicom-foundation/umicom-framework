/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_rectangular_selection.c
 *
 * PURPOSE:
 *   Implement the test rectangular selection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/rectangular_selection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbRectangularSelection s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_rectangular_selection_init(&s,5U,8U,2U,3U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s.first_line!=2U||s.first_column!=3U||umi_editor_wb_rectangular_selection_line_count(&s)!=4U)return 2; return 0; }
