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
int main(void){uint64_t a=umi_ui_ent_row_key_from_text("row-1"),b=umi_ui_ent_row_key_from_text("row-1");if(a==0U||a!=b)return 1;if(umi_ui_ent_row_key_combine(a,7U)==0U)return 2;puts("ok");return 0;}
