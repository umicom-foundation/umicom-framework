/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_device_family.c
 *
 * PURPOSE:
 *   Validate classify logical viewport/input combinations into reusable device-family semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/device_family.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveDeviceFamily family;
    CHECK(umi_adaptive_device_family_classify(390, 1, 0, &family) == UMI_STATUS_OK);
    CHECK(family == UMI_ADAPTIVE_DEVICE_HANDSET);
    CHECK(umi_adaptive_device_family_classify(1920, 0, 1, &family) == UMI_STATUS_OK);
    CHECK(family == UMI_ADAPTIVE_DEVICE_DESKTOP);
    return 0;
}
