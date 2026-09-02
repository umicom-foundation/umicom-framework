/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_expansion.c
 *
 * PURPOSE:
 *   Exercise the tree expansion enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_expansion.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntTreeExpansion e;umi_ui_ent_tree_expansion_init(&e);umi_ui_ent_tree_expansion_set(&e,"n",1);/* Apply this branch only when its contract condition is satisfied. */ if(!umi_ui_ent_tree_expansion_is_expanded(&e,"n"))return 1;umi_ui_ent_tree_expansion_set(&e,"n",0);/* Apply this branch only when its contract condition is satisfied. */ if(umi_ui_ent_tree_expansion_is_expanded(&e,"n"))return 2;puts("ok");return 0;}
