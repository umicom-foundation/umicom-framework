/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_workspace_symbol_result.c
 *
 * PURPOSE:
 *   Implement the test workspace symbol result behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/workspace_symbol_result.h"
int main(void){UmiEditorIntelWorkspaceSymbolResult model;UmiEditorIntelRange range={{3U,4U},{3U,12U}};if(umi_editor_intel_workspace_symbol_result_init(&model,"model-1","item","src/main.c",range)!=UMI_STATUS_OK)return 1;if(umi_editor_intel_workspace_symbol_result_set_score(&model,91U)!=UMI_STATUS_OK)return 2;if(umi_editor_intel_workspace_symbol_result_set_selected(&model,true)!=UMI_STATUS_OK)return 3;if(!umi_editor_intel_workspace_symbol_result_valid(&model)||model.value.score!=91U||!model.selected)return 4;return 0;}
