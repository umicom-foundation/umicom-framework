/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/surface.c
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
#include "umicom/ui/components/surface.h"
UmiUiSurface umi_ui_surface_default(void){UmiUiSurface s={-1,-1,0U};return s;}
