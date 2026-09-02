/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_safe_area_resolver.c
 *
 * PURPOSE:
 *   Validate apply runtime safe-area semantics to authored designer component rectangles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/safe_area_resolver.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRadRect in={0,0,390,100}; UmiRadRect out; UmiAdaptiveViewport v={390,844}; UmiAdaptiveInsets s={44,0,34,0};
    CHECK(umi_designer_safe_area_resolver_apply(in,v,s,&out)==UMI_STATUS_OK);
    CHECK(out.y==44&&out.height==100);
    return 0;
}
