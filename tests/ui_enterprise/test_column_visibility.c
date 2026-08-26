/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_visibility.c
 *
 * PURPOSE:
 *   Exercise the column visibility enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_visibility.h"
#include <stdio.h>
int main(void){UmiUiEntColumnVisibility v;umi_ui_ent_column_visibility_init(&v);umi_ui_ent_column_visibility_hide(&v,"pnl");if(umi_ui_ent_column_visibility_is_visible(&v,"pnl"))return 1;umi_ui_ent_column_visibility_show(&v,"pnl");if(!umi_ui_ent_column_visibility_is_visible(&v,"pnl"))return 2;puts("ok");return 0;}
