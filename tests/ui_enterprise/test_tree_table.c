/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_table.c
 *
 * PURPOSE:
 *   Exercise the tree table enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_table.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntTreeTable t;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_tree_table_init(&t,"name")!=UMI_STATUS_OK||!umi_ui_ent_tree_table_validate(&t))return 1;puts("ok");return 0;}
