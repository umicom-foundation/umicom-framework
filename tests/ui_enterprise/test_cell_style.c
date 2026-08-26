/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_style.c
 *
 * PURPOSE:
 *   Exercise the cell style enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_style.h"
#include <stdio.h>
int main(void){UmiUiEntCellStyle v;if(umi_ui_ent_cell_style_init(&v)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_cell_style_validate(&v))return 9;puts("ok");return 0;}
