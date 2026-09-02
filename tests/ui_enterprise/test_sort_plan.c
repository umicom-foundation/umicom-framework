/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_sort_plan.c
 *
 * PURPOSE:
 *   Exercise the sort plan enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_plan.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntSortPlan p;UmiUiEntSortDescriptor a,b;umi_ui_ent_sort_plan_init(&p);umi_ui_ent_sort_descriptor_init(&a);umi_ui_ent_sort_descriptor_init(&b);umi_ui_ent_copy_text(a.column_id,sizeof a.column_id,"a");umi_ui_ent_copy_text(b.column_id,sizeof b.column_id,"b");a.priority=2;a.direction=UMI_UI_ENT_SORT_ASCENDING;b.priority=1;b.direction=UMI_UI_ENT_SORT_DESCENDING;umi_ui_ent_sort_plan_add(&p,&a);umi_ui_ent_sort_plan_add(&p,&b);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p.count!=2U||p.items[0].priority!=1)return 1;puts("ok");return 0;}
