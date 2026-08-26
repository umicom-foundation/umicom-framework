/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_height.c
 *
 * PURPOSE:
 *   Exercise the row height enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_height.h"
#include <stdio.h>
int main(void){UmiUiEntRowHeight v;if(umi_ui_ent_row_height_init(&v)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_row_height_validate(&v))return 9;puts("ok");return 0;}
