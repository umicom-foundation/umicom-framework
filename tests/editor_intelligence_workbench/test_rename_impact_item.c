/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_rename_impact_item.c
 *
 * PURPOSE:
 *   Implement the test rename impact item behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/rename_impact_item.h"
int main(void){UmiEditorIntelRenameImpactItem model;UmiEditorIntelRange range={{3U,4U},{3U,12U}};if(umi_editor_intel_rename_impact_item_init(&model,"model-1","item","src/main.c",range)!=UMI_STATUS_OK)return 1;if(umi_editor_intel_rename_impact_item_set_score(&model,91U)!=UMI_STATUS_OK)return 2;if(umi_editor_intel_rename_impact_item_set_selected(&model,true)!=UMI_STATUS_OK)return 3;if(!umi_editor_intel_rename_impact_item_valid(&model)||model.value.score!=91U||!model.selected)return 4;return 0;}
