/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/selection.c
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
#include "umicom/ui/components/selection.h"
UmiUiComponentSelection umi_ui_component_selection_none(void){UmiUiComponentSelection s={0U,false};return s;}
