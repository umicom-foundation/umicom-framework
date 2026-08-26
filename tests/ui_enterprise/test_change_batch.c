/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_change_batch.c
 *
 * PURPOSE:
 *   Exercise the change batch enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/change_batch.h"
#include <stdio.h>
int main(void){UmiUiEntChangeBatch b;UmiUiEntDataDelta d={UMI_UI_ENT_DELTA_UPDATE,{0U,1U},1U};umi_ui_ent_change_batch_init(&b);if(umi_ui_ent_change_batch_add(&b,&d)!=UMI_STATUS_OK||b.count!=1U)return 1;puts("ok");return 0;}
