/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_provider.c
 *
 * PURPOSE:
 *   Exercise the data provider enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_provider.h"
#include <stdio.h>
static size_t rows(void *c){(void)c;return 2U;} static UmiStatus cell(void *c,size_t r,size_t col,UmiUiEntCellValue *out){(void)c;(void)col;umi_ui_ent_cell_value_set_integer(out,(int64_t)r);return UMI_STATUS_OK;}
int main(void){UmiUiEntDataProvider p={NULL,rows,cell,1U};UmiUiEntCellValue v;if(umi_ui_ent_data_provider_cell(&p,1U,0U,&v)!=UMI_STATUS_OK||v.integer_value!=1)return 1;if(umi_ui_ent_data_provider_cell(&p,2U,0U,&v)!=UMI_STATUS_NOT_FOUND)return 2;puts("ok");return 0;}
