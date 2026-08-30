/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_sort_descriptor.c
 *
 * PURPOSE:
 *   Exercise the sort descriptor enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_descriptor.h"
#include <stdio.h>
int main(void){UmiUiEntSortDescriptor v;if(umi_ui_ent_sort_descriptor_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"id")!=UMI_STATUS_OK)return 2;if(!umi_ui_ent_sort_descriptor_validate(&v))return 9;puts("ok");return 0;}
