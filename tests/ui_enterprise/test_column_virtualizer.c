/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_virtualizer.c
 *
 * PURPOSE:
 *   Exercise the column virtualizer enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_virtualizer.h"
#include <stdio.h>
int main(void){UmiUiEntColumnVirtualizer v={0};UmiUiEntViewport p={2U,3U,1U,2U,1U,1U};if(umi_ui_ent_column_virtualizer_update(&v,&p,10U)!=UMI_STATUS_OK)return 1;if(v.materialised.count==0U||v.generation!=1U)return 2;puts("ok");return 0;}
