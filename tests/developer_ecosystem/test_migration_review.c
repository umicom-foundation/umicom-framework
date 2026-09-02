/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_migration_review.c
 * PURPOSE: Verify required manual migration acceptance and failure blocking.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/migration_review.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEcosystemMigrationReview review;
    assert(umi_ecosystem_migration_review_begin(&review, "org.umicom.panel",
        "1.0.0", "2.0.0", true) == UMI_STATUS_OK);
    assert(umi_ecosystem_migration_review_add_step(&review, "manifest",
        "Upgrade manifest", "Move the contribution schema to version two.",
        UMI_ECOSYSTEM_MIGRATION_MANIFEST, UMI_ECOSYSTEM_REVIEW_WARNING,
        true, false) == UMI_STATUS_OK);
    assert(!umi_ecosystem_migration_review_ready(&review));
    assert(umi_ecosystem_migration_review_set_accepted(&review, "manifest",
        true) == UMI_STATUS_OK);
    assert(umi_ecosystem_migration_review_ready(&review));
    assert(umi_ecosystem_migration_review_complete_step(&review, "manifest",
        UMI_STATUS_PARSE_ERROR) == UMI_STATUS_OK);
    assert(!umi_ecosystem_migration_review_ready(&review));
    assert(review.rollback_available);
    return 0;
}
