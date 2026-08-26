/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_adaptive_validation.c
 *
 * PURPOSE:
 *   Validate aggregate blocking adaptive-designer validation issues across variants, rules and preview cases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_validation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveValidation v={0U,0U,0U,2U};
    CHECK(umi_designer_adaptive_validation_publishable(&v)); v.failed_previews=1U; CHECK(!umi_designer_adaptive_validation_publishable(&v));
    return 0;
}
