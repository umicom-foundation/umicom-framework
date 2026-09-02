/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_group_node.c
 *
 * PURPOSE:
 *   Exercise the group node enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/group_node.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntGroupNode v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_group_node_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.group_id,sizeof v.group_id,"id")!=UMI_STATUS_OK)return 2;v.row_count=1U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_group_node_validate(&v))return 9;puts("ok");return 0;}
