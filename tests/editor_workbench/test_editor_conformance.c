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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorConformance s={true,true,true,true,true,true}; /* Apply this operation only while the related capability or state is available. */ if(umi_editor_wb_editor_conformance_pass_count(&s)!=6U)return 1; /* Apply this operation only while the related capability or state is available. */ if(!umi_editor_wb_editor_conformance_ready(&s))return 2; s.search=false; /* Apply this operation only while the related capability or state is available. */ if(umi_editor_wb_editor_conformance_ready(&s))return 3; return 0; }
