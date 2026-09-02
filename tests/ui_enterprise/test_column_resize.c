/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_resize.c
 *
 * PURPOSE:
 *   Exercise the column resize enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_resize.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntColumnDescriptor c;umi_ui_ent_column_descriptor_init(&c);/* Apply this branch only when its contract condition is satisfied. */ if(umi_ui_ent_column_resize_apply(&c,120,-500)!=32)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_ui_ent_column_resize_apply(&c,120,5000)!=2048)return 2;puts("ok");return 0;}
