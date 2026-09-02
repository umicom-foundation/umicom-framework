/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_catalogue.c
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
#include "umicom/ui/components/catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){return umi_ui_component_catalogue_count()>=30U&&umi_ui_component_catalogue_contains(UMI_UI_COMPONENT_WINDOW)?0:1;}
