/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_evidence.c
 *
 * PURPOSE:
 *   Verify persist auditable foreground/background token and ratio evidence for conformance reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_evidence.h"
int main(void) {
    UmiAppearanceContrastEvidence item;
    if (umi_appearance_contrast_evidence_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_contrast_evidence_is_valid(&item)) return 2;
    return 0;
}
