/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_overview_ruler.c
 *
 * PURPOSE:
 *   Implement the test overview ruler behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/overview_ruler.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbOverviewRuler s; umi_editor_wb_overview_ruler_init(&s); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_overview_ruler_add(&s,9U,UMI_EDITOR_WB_ERROR)!=UMI_STATUS_OK||s.count!=1U)return 1; return 0; }
