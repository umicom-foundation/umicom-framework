/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/style.c
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
#include "umicom/ui/components/style.h"
#include <string.h>
UmiUiComponentStyle umi_ui_component_style_default(void){UmiUiComponentStyle v;memset(&v,0,sizeof(v));return v;}
