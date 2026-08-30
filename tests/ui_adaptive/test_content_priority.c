/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_content_priority.c
 *
 * PURPOSE:
 *   Validate represent semantic content priority and compare items for constrained adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/content_priority.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveContentPriority a,b;
    CHECK(umi_adaptive_content_priority_init(&a,"primary",10U,1)==UMI_STATUS_OK);
    CHECK(umi_adaptive_content_priority_init(&b,"secondary",100U,0)==UMI_STATUS_OK);
    CHECK(umi_adaptive_content_priority_compare(&a,&b)<0);
    return 0;
}
