/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/layout.c
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

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/layout.h"
UmiUiComponentLayout umi_ui_component_layout_default(void){UmiUiComponentLayout v={0,0,0,0,0,0,1,1,false};return v;}
