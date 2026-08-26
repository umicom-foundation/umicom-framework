/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_form_factor.c
 *
 * PURPOSE:
 *   Validate classify viewport aspect ratios for adaptive shell and preview decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/form_factor.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveFormFactor factor;
    UmiAdaptiveViewport phone = { 390, 844 };
    UmiAdaptiveViewport ultra = { 3440, 1440 };
    CHECK(umi_adaptive_form_factor_classify(phone, &factor) == UMI_STATUS_OK);
    CHECK(factor == UMI_ADAPTIVE_FORM_TALL);
    CHECK(umi_adaptive_form_factor_classify(ultra, &factor) == UMI_STATUS_OK);
    CHECK(factor == UMI_ADAPTIVE_FORM_ULTRAWIDE);
    return 0;
}
