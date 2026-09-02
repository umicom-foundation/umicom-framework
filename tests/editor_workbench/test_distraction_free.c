/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_distraction_free.c
 *
 * PURPOSE:
 *   Implement the test distraction free behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/distraction_free.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbDistractionFree s; umi_editor_wb_distraction_free_resolve(&s,true); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s.enabled||s.show_tabs||!s.show_status)return 1; return 0; }
