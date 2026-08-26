/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_pinned_rows.c
 *
 * PURPOSE:
 *   Exercise the pinned rows enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/pinned_rows.h"
#include <stdio.h>
int main(void){UmiUiEntPinnedRows p;umi_ui_ent_pinned_rows_init(&p);if(umi_ui_ent_pinned_rows_add_top(&p,7U)!=UMI_STATUS_OK||!umi_ui_ent_pinned_rows_contains(&p,7U))return 1;puts("ok");return 0;}
