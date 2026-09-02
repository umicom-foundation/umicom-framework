/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_target_size_certification.c
 *
 * PURPOSE:
 *   Verify certify resolved interactive target dimensions against modality-specific accessibility policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/target_size_certification.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceTargetSizeCertification item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_target_size_certification_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_target_size_certification_is_valid(&item)) return 2;
    item.width_dp=20.0; umi_appearance_target_size_certification_evaluate(&item); /* Apply this branch only when its contract condition is satisfied. */ if(item.passed) return 3;
    return 0;
}
