/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_snapshot.c
 *
 * PURPOSE:
 *   Exercise the data snapshot enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_snapshot.h"
#include <stdio.h>
int main(void){UmiUiEntDataSnapshot v;if(umi_ui_ent_data_snapshot_init(&v)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_data_snapshot_validate(&v))return 9;puts("ok");return 0;}
