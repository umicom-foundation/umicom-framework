/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_descriptor.c
 *
 * PURPOSE:
 *   Exercise the column descriptor enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_descriptor.h"
#include <stdio.h>
int main(void){UmiUiEntColumnDescriptor v;if(umi_ui_ent_column_descriptor_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"id")!=UMI_STATUS_OK)return 2;if(!umi_ui_ent_column_descriptor_validate(&v))return 9;puts("ok");return 0;}
