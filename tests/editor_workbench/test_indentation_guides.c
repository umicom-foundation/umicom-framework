/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_indentation_guides.c
 *
 * PURPOSE:
 *   Implement the test indentation guides behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/indentation_guides.h"
int main(void){ if(umi_editor_wb_indentation_guides_depth(12U,4U)!=3U)return 1; if(umi_editor_wb_indentation_guides_depth(4U,0U)!=0U)return 2; return 0; }
