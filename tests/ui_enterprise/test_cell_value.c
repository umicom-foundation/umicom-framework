/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_value.c
 *
 * PURPOSE:
 *   Exercise the cell value enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_value.h"
#include <stdio.h>
int main(void){UmiUiEntCellValue a,b;umi_ui_ent_cell_value_set_integer(&a,42);umi_ui_ent_cell_value_set_integer(&b,42);if(!umi_ui_ent_cell_value_equal(&a,&b))return 1;if(umi_ui_ent_cell_value_set_text(&a,"abc")!=UMI_STATUS_OK)return 2;if(a.kind!=UMI_UI_ENT_VALUE_TEXT)return 3;puts("ok");return 0;}
