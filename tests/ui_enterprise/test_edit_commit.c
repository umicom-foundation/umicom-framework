/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_edit_commit.c
 *
 * PURPOSE:
 *   Exercise the edit commit enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_commit.h"
#include <stdio.h>
int main(void){UmiUiEntEditCommit c;UmiUiEntCellValue a,b;umi_ui_ent_cell_value_set_integer(&a,1);umi_ui_ent_cell_value_set_integer(&b,2);if(umi_ui_ent_edit_commit_create(&c,1U,"x",&a,&b,4U,5U)!=UMI_STATUS_OK||!c.committed)return 1;puts("ok");return 0;}
