/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_key.c
 *
 * PURPOSE:
 *   Exercise the row key enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_key.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){uint64_t a=umi_ui_ent_row_key_from_text("row-1"),b=umi_ui_ent_row_key_from_text("row-1");/* Apply this branch only when its contract condition is satisfied. */ if(a==0U||a!=b)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_ui_ent_row_key_combine(a,7U)==0U)return 2;puts("ok");return 0;}
