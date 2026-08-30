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
int main(void){UmiUiEntGroupNode v;if(umi_ui_ent_group_node_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.group_id,sizeof v.group_id,"id")!=UMI_STATUS_OK)return 2;v.row_count=1U;if(!umi_ui_ent_group_node_validate(&v))return 9;puts("ok");return 0;}
