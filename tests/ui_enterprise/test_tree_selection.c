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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntTreeSelection s;umi_ui_ent_tree_selection_init(&s,UMI_UI_ENT_SELECTION_SINGLE);umi_ui_ent_tree_selection_add(&s,"a");umi_ui_ent_tree_selection_add(&s,"b");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s.count!=1U||!umi_ui_ent_tree_selection_contains(&s,"b"))return 1;puts("ok");return 0;}
