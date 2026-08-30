/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_filter_plan.c
 *
 * PURPOSE:
 *   Exercise the filter plan enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/filter_plan.h"
#include <stdio.h>
int main(void){UmiUiEntFilterPlan p;UmiUiEntFilterPredicate f={0};UmiUiEntCellValue v;umi_ui_ent_filter_plan_init(&p);umi_ui_ent_copy_text(f.column_id,sizeof f.column_id,"x");f.op=UMI_UI_ENT_FILTER_EQUALS;umi_ui_ent_cell_value_set_integer(&f.operand,7);umi_ui_ent_filter_plan_add(&p,&f);umi_ui_ent_cell_value_set_integer(&v,7);if(!umi_ui_ent_filter_plan_matches_one(&p,"x",&v))return 1;puts("ok");return 0;}
