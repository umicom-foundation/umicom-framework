/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_conformance.c
 *
 * PURPOSE:
 *   Implement the test editor conformance behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_conformance.h"
int main(void){ UmiEditorWbEditorConformance s={true,true,true,true,true,true}; if(umi_editor_wb_editor_conformance_pass_count(&s)!=6U)return 1; if(!umi_editor_wb_editor_conformance_ready(&s))return 2; s.search=false; if(umi_editor_wb_editor_conformance_ready(&s))return 3; return 0; }
