/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_focus_model.c
 *
 * PURPOSE:
 *   Exercise the focus model enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/focus_model.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntFocusModel m={0};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_focus_model_set(&m,2U,3U,5U,5U)!=UMI_STATUS_OK||m.row!=2U||m.column!=3U)return 1;puts("ok");return 0;}
