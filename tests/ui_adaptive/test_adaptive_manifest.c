/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_adaptive_manifest.c
 *
 * PURPOSE:
 *   Validate declare application-wide adaptive shell capabilities and renderer coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/adaptive_manifest.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveManifest manifest;
    CHECK(umi_adaptive_manifest_init(&manifest,"studio","studio.shell")==UMI_STATUS_OK);
    CHECK(umi_adaptive_manifest_valid(&manifest));
    return 0;
}
