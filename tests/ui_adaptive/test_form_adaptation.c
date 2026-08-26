/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_form_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt form presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/form_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveFormDecision d = umi_adaptive_form_adaptation_resolve(UMI_DESIGN_SIZE_WIDE, UMI_ADAPTIVE_INPUT_POINTER);
    CHECK(d.columns == 4U && !d.labels_above);
    return 0;
}
