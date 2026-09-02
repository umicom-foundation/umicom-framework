/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_model.c
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
#include "umicom/ui/components/model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiComponentModel m=umi_ui_component_model_default(UMI_UI_COMPONENT_ENTRY);return m.state.visible&&m.state.enabled?0:1;}
