/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_media_adaptation.c
 *
 * PURPOSE:
 *   Validate resolve media-preview and transport placement for adaptive creative application layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/media_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerMediaAdaptation m=umi_designer_media_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT);
    CHECK(m.transport_overlay&&m.inspector_overlay&&m.preview_fraction>0.69);
    return 0;
}
