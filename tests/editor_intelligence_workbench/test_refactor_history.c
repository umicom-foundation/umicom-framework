/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_history.c
 *
 * PURPOSE:
 *   Implement the test refactor history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_history.h"
int main(void){UmiEditorIntelRefactorHistory model;UmiEditorIntelEntry entry;UmiEditorIntelRange range={{1U,2U},{1U,8U}};if(umi_editor_intel_refactor_history_init(&model)!=UMI_STATUS_OK)return 1;if(umi_editor_intel_entry_init(&entry,"id-1","entry","src/a.c",range)!=UMI_STATUS_OK)return 2;if(umi_editor_intel_refactor_history_add(&model,&entry)!=UMI_STATUS_OK)return 3;if(umi_editor_intel_refactor_history_add(&model,&entry)!=UMI_STATUS_ALREADY_EXISTS)return 4;if(umi_editor_intel_refactor_history_find(&model,"id-1")==NULL||!umi_editor_intel_refactor_history_valid(&model))return 5;if(umi_editor_intel_refactor_history_clear(&model)!=UMI_STATUS_OK||model.count!=0U)return 6;return 0;}
