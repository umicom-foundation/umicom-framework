/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_keyboard_navigation.c
 *
 * PURPOSE:
 *   Exercise the keyboard navigation enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/keyboard_navigation.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntNavigationPosition p={3U,2U};p=umi_ui_ent_keyboard_navigation_apply(p,UMI_UI_ENT_NAV_PAGE_DOWN,10U,4U,4U);/* Apply this branch only when its contract condition is satisfied. */ if(p.row!=7U)return 1;p=umi_ui_ent_keyboard_navigation_apply(p,UMI_UI_ENT_NAV_END,10U,4U,4U);/* Apply this branch only when its contract condition is satisfied. */ if(p.column!=3U)return 2;puts("ok");return 0;}
