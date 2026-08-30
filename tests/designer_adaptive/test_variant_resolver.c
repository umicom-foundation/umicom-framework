/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_variant_resolver.c
 *
 * PURPOSE:
 *   Validate choose the highest-priority adaptive variant matching size class and orientation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_resolver.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveVariantSet set; UmiDesignerAdaptiveVariant a,b,out;
    umi_designer_adaptive_variant_set_init(&set);
    CHECK(umi_designer_adaptive_variant_init(&a,"a","","compact",UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,1U)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_init(&b,"b","","compact",UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,9U)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_set_add(&set,&a)==UMI_STATUS_OK); CHECK(umi_designer_adaptive_variant_set_add(&set,&b)==UMI_STATUS_OK);
    CHECK(umi_designer_variant_resolver_resolve(&set,UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,&out)==UMI_STATUS_OK);
    CHECK(out.priority==9U);
    return 0;
}
