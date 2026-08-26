/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_frozen_columns.c
 *
 * PURPOSE:
 *   Exercise the frozen columns enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/frozen_columns.h"
#include <stdio.h>
int main(void){UmiUiEntFrozenColumns v;if(umi_ui_ent_frozen_columns_init(&v)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_frozen_columns_validate(&v))return 9;puts("ok");return 0;}
