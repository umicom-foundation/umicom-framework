/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_symbol_breadcrumb.c
 *
 * PURPOSE:
 *   Implement the test symbol breadcrumb behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/symbol_breadcrumb.h"
int main(void){ UmiEditorWbSymbolBreadcrumb s; umi_editor_wb_symbol_breadcrumb_init(&s); if(umi_editor_wb_symbol_breadcrumb_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_symbol_breadcrumb_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; if(umi_editor_wb_symbol_breadcrumb_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
