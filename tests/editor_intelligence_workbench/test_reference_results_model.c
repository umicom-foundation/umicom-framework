/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_reference_results_model.c
 *
 * PURPOSE:
 *   Implement the test reference results model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/reference_results_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelReferenceResultsModel model;UmiEditorIntelEntry entry;UmiEditorIntelRange range={{1U,2U},{1U,8U}};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_reference_results_model_init(&model)!=UMI_STATUS_OK)return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_entry_init(&entry,"id-1","entry","src/a.c",range)!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_reference_results_model_add(&model,&entry)!=UMI_STATUS_OK)return 3;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_reference_results_model_add(&model,&entry)!=UMI_STATUS_ALREADY_EXISTS)return 4;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_reference_results_model_find(&model,"id-1")==NULL||!umi_editor_intel_reference_results_model_valid(&model))return 5;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_reference_results_model_clear(&model)!=UMI_STATUS_OK||model.count!=0U)return 6;return 0;}
