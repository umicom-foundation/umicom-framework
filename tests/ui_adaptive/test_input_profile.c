/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_input_profile.c
 *
 * PURPOSE:
 *   Validate describe pointer, touch, keyboard and hover capabilities independently of a toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/input_profile.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveInputProfile profile;
    CHECK(umi_adaptive_input_profile_init(&profile, 1, 1, 1, 1, 1) == UMI_STATUS_OK);
    CHECK(umi_adaptive_input_profile_primary(&profile) == UMI_ADAPTIVE_INPUT_HYBRID);
    return 0;
}
