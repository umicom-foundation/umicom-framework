/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_validation.c
 *
 * PURPOSE:
 *   Exercise the cell validation enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_validation.h"
#include <stdio.h>
int main(void){UmiUiEntCellValidationRule r={1,0U,1,1,0.0,10.0};UmiUiEntCellValue v;char m[64];umi_ui_ent_cell_value_set_real(&v,12.0);if(umi_ui_ent_cell_validation_validate(&r,&v,m,sizeof m)==UMI_STATUS_OK)return 1;umi_ui_ent_cell_value_set_real(&v,5.0);if(umi_ui_ent_cell_validation_validate(&r,&v,m,sizeof m)!=UMI_STATUS_OK)return 2;puts("ok");return 0;}
