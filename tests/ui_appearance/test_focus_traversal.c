/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_focus_traversal.c
 *
 * PURPOSE:
 *   Verify maintain deterministic keyboard focus order independently of toolkit widget hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_traversal.h"
#include <string.h>
int main(void){UmiAppearanceFocusTraversal o;if(umi_appearance_focus_traversal_init(&o)!=UMI_STATUS_OK)return 1;if(umi_appearance_focus_traversal_add(&o,"a")!=UMI_STATUS_OK||umi_appearance_focus_traversal_add(&o,"b")!=UMI_STATUS_OK)return 2;if(strcmp(umi_appearance_focus_traversal_next(&o,"a",1),"b")!=0)return 3;if(strcmp(umi_appearance_focus_traversal_next(&o,"b",1),"a")!=0)return 4;return 0;}
