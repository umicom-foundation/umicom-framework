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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntTreePath p;umi_ui_ent_tree_path_init(&p);umi_ui_ent_tree_path_append(&p,"root");umi_ui_ent_tree_path_append(&p,"child");umi_ui_ent_tree_path_parent(&p);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p.count!=1U)return 1;puts("ok");return 0;}
