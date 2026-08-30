/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_path.c
 *
 * PURPOSE:
 *   Exercise the tree path enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_path.h"
#include <stdio.h>
int main(void){UmiUiEntTreePath p;umi_ui_ent_tree_path_init(&p);umi_ui_ent_tree_path_append(&p,"root");umi_ui_ent_tree_path_append(&p,"child");umi_ui_ent_tree_path_parent(&p);if(p.count!=1U)return 1;puts("ok");return 0;}
