/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_lazy_loader.c
 *
 * PURPOSE:
 *   Exercise the tree lazy loader enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_lazy_loader.h"
#include <stdio.h>
int main(void){UmiUiEntTreeLazyLoader l={0};if(umi_ui_ent_tree_lazy_loader_request(&l,"root",4U)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_tree_lazy_loader_complete(&l,4U,1)!=UMI_STATUS_OK||l.state!=UMI_UI_ENT_LOAD_LOADED)return 2;puts("ok");return 0;}
