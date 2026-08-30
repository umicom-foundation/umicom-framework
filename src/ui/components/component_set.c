/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/component_set.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/component_set.h"
void umi_ui_component_set_init(UmiUiComponentSet*s){if(!s)return;umi_ui_component_registry_init(&s->registry);umi_ui_component_tree_init(&s->tree);}
