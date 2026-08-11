/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/state.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/state.h"
UmiUiComponentState umi_ui_component_state_default(void){UmiUiComponentState v={true,true,false,false};return v;}
