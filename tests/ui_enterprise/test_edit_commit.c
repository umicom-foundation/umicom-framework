/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_edit_commit.c
 *
 * PURPOSE:
 *   Exercise the edit commit enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_commit.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntEditCommit c;UmiUiEntCellValue a,b;umi_ui_ent_cell_value_set_integer(&a,1);umi_ui_ent_cell_value_set_integer(&b,2);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_edit_commit_create(&c,1U,"x",&a,&b,4U,5U)!=UMI_STATUS_OK||!c.committed)return 1;puts("ok");return 0;}
