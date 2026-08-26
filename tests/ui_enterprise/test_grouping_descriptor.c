/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_grouping_descriptor.c
 *
 * PURPOSE:
 *   Exercise the grouping descriptor enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/grouping_descriptor.h"
#include <stdio.h>
int main(void){UmiUiEntGroupingDescriptor v;if(umi_ui_ent_grouping_descriptor_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"id")!=UMI_STATUS_OK)return 2;if(!umi_ui_ent_grouping_descriptor_validate(&v))return 9;puts("ok");return 0;}
