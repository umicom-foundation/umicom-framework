/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_accessibility.c
 *
 * PURPOSE:
 *   Implement the test editor accessibility behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_accessibility.h"
int main(void){ UmiEditorWbEditorAccessibility s; if(umi_editor_wb_editor_accessibility_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_accessibility_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_accessibility_valid(&s)||s.primary!=3U)return 3; return 0; }
