/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_selection.c
 *
 * PURPOSE:
 *   Exercise the tree selection enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_selection.h"
#include <stdio.h>
int main(void){UmiUiEntTreeSelection s;umi_ui_ent_tree_selection_init(&s,UMI_UI_ENT_SELECTION_SINGLE);umi_ui_ent_tree_selection_add(&s,"a");umi_ui_ent_tree_selection_add(&s,"b");if(s.count!=1U||!umi_ui_ent_tree_selection_contains(&s,"b"))return 1;puts("ok");return 0;}
