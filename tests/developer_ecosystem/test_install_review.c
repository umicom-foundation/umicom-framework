/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_install_review.c
 * PURPOSE: Verify required approvals and blocker-aware install readiness.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/install_review.h"

int main(void)
{
    UmiEcosystemPackageRecord record;
    UmiEcosystemInstallReview review;
    umi_ecosystem_package_record_init(&record, "org.umicom.extension", "Extension",
        UMI_ECOSYSTEM_PACKAGE_EXTENSION, "official", "1.0.0");
    record.compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    assert(umi_ecosystem_install_review_begin(&review, &record) == UMI_STATUS_OK);
    assert(umi_ecosystem_install_review_add_action(&review,
        UMI_ECOSYSTEM_ACTION_REVIEW_PERMISSIONS, record.package_id,
        "Review file-system permission.", UMI_ECOSYSTEM_REVIEW_WARNING,
        true, true) == UMI_STATUS_OK);
    assert(!umi_ecosystem_install_review_ready(&review));
    assert(review.pending_approval_count == 1U);
    assert(umi_ecosystem_install_review_set_approved(&review, 0U, true) ==
        UMI_STATUS_OK);
    assert(umi_ecosystem_install_review_ready(&review));
    assert(umi_ecosystem_install_review_complete_action(&review, 0U,
        UMI_STATUS_IO_ERROR) == UMI_STATUS_OK);
    assert(!umi_ecosystem_install_review_ready(&review));
    return 0;
}
