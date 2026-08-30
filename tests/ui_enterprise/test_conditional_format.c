/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_conditional_format.c
 *
 * PURPOSE:
 *   Exercise the conditional format enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/conditional_format.h"
#include <stdio.h>
int main(void){UmiUiEntConditionalFormat r={UMI_UI_ENT_COMPARE_GT,10.0,"warning"};UmiUiEntCellValue v;umi_ui_ent_cell_value_set_real(&v,12.0);if(!umi_ui_ent_conditional_format_matches(&r,&v))return 1;puts("ok");return 0;}
