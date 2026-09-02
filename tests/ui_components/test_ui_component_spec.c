/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_spec.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include <string.h>
#include "umicom/ui/components/component.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiComponentSpec s=umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_set_id(&s,"save")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_set_text(&s,"Save")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_validate(&s)!=UMI_STATUS_OK)return 3;return strcmp(umi_ui_component_kind_name(s.kind),"button")==0?0:4;}
