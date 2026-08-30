/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_conformance.c
 *
 * PURPOSE:
 *   Verify define release-gate requirements for theme, high-DPI, accessibility and frontend appearance parity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_conformance.h"
int main(void) {
    UmiAppearanceAppearanceConformance item;
    if (umi_appearance_conformance_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_conformance_is_valid(&item)) return 2;
    return 0;
}
