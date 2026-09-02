/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_lazy_loader.c
 *
 * PURPOSE:
 *   Exercise the tree lazy loader enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_lazy_loader.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntTreeLazyLoader l={0};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_tree_lazy_loader_request(&l,"root",4U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_tree_lazy_loader_complete(&l,4U,1)!=UMI_STATUS_OK||l.state!=UMI_UI_ENT_LOAD_LOADED)return 2;puts("ok");return 0;}
