/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_order.c
 *
 * PURPOSE:
 *   Exercise the column order enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_order.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntColumnOrder o;umi_ui_ent_column_order_init(&o);umi_ui_ent_column_order_append(&o,"a");umi_ui_ent_column_order_append(&o,"b");umi_ui_ent_column_order_move(&o,0U,1U);/* Apply this branch only when its contract condition is satisfied. */ if(o.ids[0][0]!='b')return 1;puts("ok");return 0;}
