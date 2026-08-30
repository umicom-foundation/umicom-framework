/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_viewport.c
 *
 * PURPOSE:
 *   Exercise the viewport enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/viewport.h"
#include <stdio.h>
int main(void){UmiUiEntViewport v={10U,5U,2U,3U,2U,1U};UmiUiEntSpan s=umi_ui_ent_viewport_row_span(&v,100U);if(s.first!=8U||s.count!=9U)return 1;puts("ok");return 0;}
