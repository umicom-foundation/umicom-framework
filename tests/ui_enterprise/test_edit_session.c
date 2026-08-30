/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_edit_session.c
 *
 * PURPOSE:
 *   Exercise the edit session enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_session.h"
#include <stdio.h>
int main(void){UmiUiEntEditSession s;UmiUiEntCellValue a,b;umi_ui_ent_cell_value_set_integer(&a,1);umi_ui_ent_cell_value_set_integer(&b,2);if(umi_ui_ent_edit_session_begin(&s,3U,"amount",&a)!=UMI_STATUS_OK)return 1;umi_ui_ent_edit_session_update(&s,&b);if(!s.dirty)return 2;umi_ui_ent_edit_session_cancel(&s);if(s.dirty||s.active)return 3;puts("ok");return 0;}
