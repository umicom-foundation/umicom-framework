/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_target_size_certification.c
 *
 * PURPOSE:
 *   Verify certify resolved interactive target dimensions against modality-specific accessibility policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/target_size_certification.h"
int main(void) {
    UmiAppearanceTargetSizeCertification item;
    if (umi_appearance_target_size_certification_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_target_size_certification_is_valid(&item)) return 2;
    item.width_dp=20.0; umi_appearance_target_size_certification_evaluate(&item); if(item.passed) return 3;
    return 0;
}
