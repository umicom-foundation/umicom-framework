/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_delta.c
 *
 * PURPOSE:
 *   Exercise the data delta enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_delta.h"
#include <stdio.h>
int main(void){UmiUiEntDataDelta d={UMI_UI_ENT_DELTA_UPDATE,{5U,2U},1U};if(!umi_ui_ent_data_delta_validate(&d)||!umi_ui_ent_data_delta_touches(&d,6U)||umi_ui_ent_data_delta_touches(&d,7U))return 1;puts("ok");return 0;}
