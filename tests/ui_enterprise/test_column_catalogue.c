/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_column_catalogue.c
 *
 * PURPOSE:
 *   Exercise the column catalogue enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_catalogue.h"
#include <stdio.h>
int main(void){UmiUiEntColumnCatalogue c;UmiUiEntColumnDescriptor a,b;umi_ui_ent_column_catalogue_init(&c);umi_ui_ent_column_descriptor_init(&a);umi_ui_ent_column_descriptor_init(&b);umi_ui_ent_copy_text(a.column_id,sizeof a.column_id,"a");umi_ui_ent_copy_text(b.column_id,sizeof b.column_id,"b");if(umi_ui_ent_column_catalogue_add(&c,&a)!=UMI_STATUS_OK||umi_ui_ent_column_catalogue_add(&c,&b)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_column_catalogue_move(&c,0U,1U)!=UMI_STATUS_OK||c.items[0].column_id[0]!='b')return 2;puts("ok");return 0;}
