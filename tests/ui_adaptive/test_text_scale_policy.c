/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_text_scale_policy.c
 *
 * PURPOSE:
 *   Validate clamp user text scaling to accessible, renderer-neutral application limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/text_scale_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    double scale;
    CHECK(umi_adaptive_text_scale_policy_resolve(1.0, 1.5, &scale) == UMI_STATUS_OK);
    CHECK(scale > 1.49 && scale < 1.51);
    return 0;
}
