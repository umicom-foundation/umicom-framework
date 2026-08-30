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
int main(void){ UmiEditorWbDistractionFree s; umi_editor_wb_distraction_free_resolve(&s,true); if(!s.enabled||s.show_tabs||!s.show_status)return 1; return 0; }
