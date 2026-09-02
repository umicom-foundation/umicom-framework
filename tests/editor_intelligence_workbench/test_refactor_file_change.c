/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_file_change.c
 *
 * PURPOSE:
 *   Implement the test refactor file change behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_file_change.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelRefactorFileChange model;UmiEditorIntelRange range={{3U,4U},{3U,12U}};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_file_change_init(&model,"model-1","item","src/main.c",range)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_file_change_set_score(&model,91U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_file_change_set_selected(&model,true)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_file_change_valid(&model)||model.value.score!=91U||!model.selected)return 4;return 0;}
