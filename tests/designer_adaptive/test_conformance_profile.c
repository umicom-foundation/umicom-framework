/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_conformance_profile.c
 *
 * PURPOSE:
 *   Validate declare semantic adaptive-layout expectations shared across gtk4, qt6 and native web previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/conformance_profile.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerAdaptiveConformanceProfile p; CHECK(umi_designer_adaptive_conformance_profile_default(UMI_RAD_TARGET_NATIVE_WEB,&p)==UMI_STATUS_OK); CHECK(umi_designer_adaptive_conformance_profile_complete(&p));
    return 0;
}
