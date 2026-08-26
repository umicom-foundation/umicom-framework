/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_descriptor.c
 *
 * PURPOSE:
 *   Exercise the row descriptor enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_descriptor.h"
#include <stdio.h>
int main(void){UmiUiEntRowDescriptor v;if(umi_ui_ent_row_descriptor_init(&v)!=UMI_STATUS_OK)return 1;v.row_key=42U;if(!umi_ui_ent_row_descriptor_validate(&v))return 9;puts("ok");return 0;}
