/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_certification.c
 *
 * PURPOSE:
 *   Verify certify measured Design-System contrast ratios against policy thresholds without duplicating colour science.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_certification.h"
int main(void) {
    UmiAppearanceContrastCertification item;
    if (umi_appearance_contrast_certification_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_contrast_certification_is_valid(&item)) return 2;
    if (umi_appearance_contrast_certification_evaluate(&item,3.0,4.5)!=UMI_STATUS_OK || item.passed) return 3;
    return 0;
}
