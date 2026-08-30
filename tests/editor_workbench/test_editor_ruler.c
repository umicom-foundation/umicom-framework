/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_ruler.c
 *
 * PURPOSE:
 *   Implement the test editor ruler behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_ruler.h"
int main(void){ UmiEditorWbEditorRuler s; umi_editor_wb_editor_ruler_init(&s); if(umi_editor_wb_editor_ruler_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_ruler_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; if(umi_editor_wb_editor_ruler_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
