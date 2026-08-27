/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_update_review.c
 * PURPOSE: Verify safe update selection, pinning and rollout summaries.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/update_review.h"

static void make_update(
    UmiEcosystemPackageRecord *record,
    const char *id,
    UmiEcosystemEvidenceState evidence,
    bool pinned)
{
    umi_ecosystem_package_record_init(record, id, id,
        UMI_ECOSYSTEM_PACKAGE_COMPONENT, "official", "2.0.0");
    assert(umi_ecosystem_copy_text(record->installed_version,
        sizeof(record->installed_version), "1.0.0") == UMI_STATUS_OK);
    record->installed = true;
    record->state = UMI_ECOSYSTEM_UPDATE_AVAILABLE;
    record->compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    record->evidence = evidence;
    record->pinned = pinned;
    record->download_bytes = 100U;
}

int main(void)
{
    UmiEcosystemUpdateReview review;
    UmiEcosystemPackageRecord safe;
    UmiEcosystemPackageRecord pinned;
    make_update(&safe, "safe", UMI_ECOSYSTEM_EVIDENCE_VERIFIED, false);
    make_update(&pinned, "pinned", UMI_ECOSYSTEM_EVIDENCE_VERIFIED, true);
    umi_ecosystem_update_review_init(&review);
    assert(umi_ecosystem_update_review_add(&review, &safe) == UMI_STATUS_OK);
    assert(umi_ecosystem_update_review_add(&review, &pinned) == UMI_STATUS_OK);
    assert(umi_ecosystem_update_review_select_safe(&review) == 1U);
    assert(review.selected_count == 1U && review.blocked_count == 1U);
    assert(review.selected_download_bytes == 100U);
    assert(umi_ecosystem_update_review_set_selected(&review, "pinned", true) ==
        UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ecosystem_update_review_ready(&review));
    return 0;
}
