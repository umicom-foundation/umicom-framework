/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_validation.c
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
#include "umicom/ui/components/validation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_validate_parent_child(UMI_UI_COMPONENT_BUTTON,UMI_UI_COMPONENT_LABEL)==UMI_STATUS_OK)return 1;return umi_ui_component_validate_parent_child(UMI_UI_COMPONENT_BOX,UMI_UI_COMPONENT_LABEL)==UMI_STATUS_OK?0:2;}
