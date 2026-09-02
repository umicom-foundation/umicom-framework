/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_edit_history.c
 *
 * PURPOSE:
 *   Exercise the edit history enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_history.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntEditHistory h;UmiUiEntEditCommit c={0};c.committed=1;c.row_key=1U;umi_ui_ent_edit_history_init(&h);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_edit_history_push(&h,&c)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_edit_history_undo(&h)||!umi_ui_ent_edit_history_redo(&h))return 2;puts("ok");return 0;}
