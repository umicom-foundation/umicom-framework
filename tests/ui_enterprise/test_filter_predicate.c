/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_filter_predicate.c
 *
 * PURPOSE:
 *   Exercise the filter predicate enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/filter_predicate.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntFilterPredicate p={0};UmiUiEntCellValue v;umi_ui_ent_copy_text(p.column_id,sizeof p.column_id,"name");p.op=UMI_UI_ENT_FILTER_CONTAINS;umi_ui_ent_cell_value_set_text(&p.operand,"fin");umi_ui_ent_cell_value_set_text(&v,"Finance");/* Use the stable identifier comparison to choose the matching record or policy. */ if(!umi_ui_ent_filter_predicate_matches(&p,&v))return 1;puts("ok");return 0;}
