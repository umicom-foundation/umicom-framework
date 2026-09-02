/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_hit_test.c
 *
 * PURPOSE:
 *   Exercise the hit test enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/hit_test.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntHit h=umi_ui_ent_hit_test_uniform(125,45,20,60,10U,2U,100U,10U);/* Apply this operation only while the related capability or state is available. */ if(!h.valid||h.row!=12U||h.column!=4U)return 1;puts("ok");return 0;}
