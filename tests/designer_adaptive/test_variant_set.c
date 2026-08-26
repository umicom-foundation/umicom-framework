/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_variant_set.c
 *
 * PURPOSE:
 *   Validate maintain a bounded unique collection of adaptive design variants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_set.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveVariantSet set; UmiDesignerAdaptiveVariant v;
    umi_designer_adaptive_variant_set_init(&set);
    CHECK(umi_designer_adaptive_variant_init(&v,"phone","","compact",UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,100U)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_set_add(&set,&v)==UMI_STATUS_OK);
    CHECK(umi_designer_adaptive_variant_set_find(&set,"phone")!=NULL);
    return 0;
}
