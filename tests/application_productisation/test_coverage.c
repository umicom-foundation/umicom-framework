/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_coverage.c
 *
 * PURPOSE:
 *   Verify portfolio and per-product coverage respond to accepted evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/coverage.h"
#include "umicom/test_runtime/check.h"

int main(void)
{
    UmiProductisationEvidenceLedger *ledger =
        (UmiProductisationEvidenceLedger *)calloc(1U, sizeof(*ledger));
    UmiProductPortfolioCoverage *before =
        (UmiProductPortfolioCoverage *)calloc(1U, sizeof(*before));
    UmiProductPortfolioCoverage *after =
        (UmiProductPortfolioCoverage *)calloc(1U, sizeof(*after));
    const UmiProductApplicationCoverage *studio_before;
    const UmiProductApplicationCoverage *studio_after;
    const UmiApplicationExperienceDefinition *studio_experience;

    UMI_TEST_REQUIRE(ledger != NULL && before != NULL && after != NULL);
    umi_productisation_evidence_ledger_init(ledger);
    UMI_TEST_REQUIRE(umi_product_portfolio_coverage_build(ledger, before) ==
                     UMI_STATUS_OK);
    studio_before = umi_product_portfolio_coverage_find(
        before, "org.umicom.studio");
    studio_experience = umi_application_experience_catalogue_find(
        "org.umicom.studio");
    UMI_TEST_REQUIRE(studio_before != NULL && studio_experience != NULL);
    /* Coverage counts every canonical feature, panel and layout exactly once. */
    UMI_TEST_REQUIRE(studio_before->asset_count ==
                     studio_experience->feature_count +
                     studio_experience->panel_count +
                     studio_experience->layout_count);
    UMI_TEST_REQUIRE(umi_productisation_evidence_record(
        ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", "gtk", UMI_PRODUCTISATION_EVIDENCE_ACCEPTED,
        "studio.editor.acceptance") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_portfolio_coverage_build(ledger, after) ==
                     UMI_STATUS_OK);
    studio_after = umi_product_portfolio_coverage_find(
        after, "org.umicom.studio");
    UMI_TEST_REQUIRE(studio_after != NULL);
    UMI_TEST_REQUIRE(studio_after->accepted_count >
                     studio_before->accepted_count);
    UMI_TEST_REQUIRE(studio_after->coverage_percent >
                     studio_before->coverage_percent);
    free(after);
    free(before);
    free(ledger);
    return 0;
}
