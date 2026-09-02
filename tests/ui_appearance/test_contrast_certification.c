/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_certification.c
 *
 * PURPOSE:
 *   Verify certify measured Design-System contrast ratios against policy thresholds without duplicating colour science.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_certification.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceContrastCertification item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_contrast_certification_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_contrast_certification_is_valid(&item)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_contrast_certification_evaluate(&item,3.0,4.5)!=UMI_STATUS_OK || item.passed) return 3;
    return 0;
}
