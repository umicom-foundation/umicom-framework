/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_breakpoint_inheritance.c
 *
 * PURPOSE:
 *   Validate resolve bounded parent chains for adaptive variants without accepting inheritance cycles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/breakpoint_inheritance.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerAdaptiveVariantSet set; UmiDesignerAdaptiveVariant base,phone; UmiDesignerBreakpointInheritance chain;
    umi_designer_adaptive_variant_set_init(&set);
    CHECK(umi_designer_adaptive_variant_init(&base,"base","","wide",UMI_DESIGN_SIZE_WIDE,UMI_ADAPTIVE_ORIENTATION_LANDSCAPE,1U)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_init(&phone,"phone","base","compact",UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,2U)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_set_add(&set,&base)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_set_add(&set,&phone)==UMI_STATUS_OK);
    CHECK(umi_designer_breakpoint_inheritance_resolve(&set,"phone",&chain)==UMI_STATUS_OK);
    CHECK(chain.count==2U);
    return 0;
}
