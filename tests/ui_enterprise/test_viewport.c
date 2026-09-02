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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntViewport v={10U,5U,2U,3U,2U,1U};UmiUiEntSpan s=umi_ui_ent_viewport_row_span(&v,100U);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s.first!=8U||s.count!=9U)return 1;puts("ok");return 0;}
