/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_rename_impact_summary.c
 *
 * PURPOSE:
 *   Implement the test rename impact summary behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/rename_impact_summary.h"
int main(void){UmiEditorIntelRenameImpactSummary model;UmiEditorIntelEntry entry;UmiEditorIntelRange range={{1U,2U},{1U,8U}};if(umi_editor_intel_rename_impact_summary_init(&model)!=UMI_STATUS_OK)return 1;if(umi_editor_intel_entry_init(&entry,"id-1","entry","src/a.c",range)!=UMI_STATUS_OK)return 2;if(umi_editor_intel_rename_impact_summary_add(&model,&entry)!=UMI_STATUS_OK)return 3;if(umi_editor_intel_rename_impact_summary_add(&model,&entry)!=UMI_STATUS_ALREADY_EXISTS)return 4;if(umi_editor_intel_rename_impact_summary_find(&model,"id-1")==NULL||!umi_editor_intel_rename_impact_summary_valid(&model))return 5;if(umi_editor_intel_rename_impact_summary_clear(&model)!=UMI_STATUS_OK||model.count!=0U)return 6;return 0;}
