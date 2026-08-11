/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_spec.c
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
#include <string.h>
#include "umicom/ui/components/component.h"
int main(void){UmiUiComponentSpec s=umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);if(umi_ui_component_spec_set_id(&s,"save")!=UMI_STATUS_OK)return 1;if(umi_ui_component_spec_set_text(&s,"Save")!=UMI_STATUS_OK)return 2;if(umi_ui_component_spec_validate(&s)!=UMI_STATUS_OK)return 3;return strcmp(umi_ui_component_kind_name(s.kind),"button")==0?0:4;}
