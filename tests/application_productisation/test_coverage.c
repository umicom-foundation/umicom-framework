/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_coverage.c
 *
 * PURPOSE:
 *   Verify portfolio and per-product coverage respond to accepted evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/productisation/coverage.h"

int main(void)
{
    UmiProductisationEvidenceLedger ledger;
    UmiProductPortfolioCoverage before;
    UmiProductPortfolioCoverage after;
    const UmiProductApplicationCoverage *studio_before;
    const UmiProductApplicationCoverage *studio_after;
    umi_productisation_evidence_ledger_init(&ledger);
    assert(umi_product_portfolio_coverage_build(&ledger, &before) ==
           UMI_STATUS_OK);
    studio_before = umi_product_portfolio_coverage_find(
        &before, "org.umicom.studio");
    assert(studio_before != NULL && studio_before->asset_count == 24U);
    assert(umi_productisation_evidence_record(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", "gtk", UMI_PRODUCTISATION_EVIDENCE_ACCEPTED,
        "studio.editor.acceptance") == UMI_STATUS_OK);
    assert(umi_product_portfolio_coverage_build(&ledger, &after) ==
           UMI_STATUS_OK);
    studio_after = umi_product_portfolio_coverage_find(
        &after, "org.umicom.studio");
    assert(studio_after != NULL);
    assert(studio_after->accepted_count > studio_before->accepted_count);
    assert(studio_after->coverage_percent > studio_before->coverage_percent);
    return 0;
}
