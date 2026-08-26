/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_cache.c
 *
 * PURPOSE:
 *   Exercise the cell cache enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_cache.h"
#include <stdio.h>
int main(void){UmiUiEntCellCache c;UmiUiEntCellValue v,o;umi_ui_ent_cell_cache_init(&c);umi_ui_ent_cell_value_set_integer(&v,5);if(umi_ui_ent_cell_cache_put(&c,1U,2U,&v)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_cell_cache_get(&c,1U,2U,&o)||o.integer_value!=5)return 2;puts("ok");return 0;}
