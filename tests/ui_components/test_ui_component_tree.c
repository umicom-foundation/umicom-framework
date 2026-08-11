/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_tree.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/tree.h"
int main(void){UmiUiComponentTree t;umi_ui_component_tree_init(&t);UmiUiComponentSpec a=umi_ui_component_spec_default(UMI_UI_COMPONENT_BOX);UmiUiComponentSpec b=umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);(void)umi_ui_component_spec_set_id(&a,"root");(void)umi_ui_component_spec_set_id(&b,"child");size_t root=0;if(umi_ui_component_tree_add(&t,&a,-1,&root)!=UMI_STATUS_OK)return 1;if(umi_ui_component_tree_add(&t,&b,(int32_t)root,NULL)!=UMI_STATUS_OK)return 2;return umi_ui_component_tree_child_count(&t,root)==1U?0:3;}
