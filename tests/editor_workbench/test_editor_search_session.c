/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_search_session.c
 *
 * PURPOSE:
 *   Implement the test editor search session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_search_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorSearchSession s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_search_session_init(&s,"foo")!=UMI_STATUS_OK)return 1; (void)umi_editor_wb_editor_search_session_set_matches(&s,3U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_search_session_next(&s,-1)!=2U)return 2; return 0; }
