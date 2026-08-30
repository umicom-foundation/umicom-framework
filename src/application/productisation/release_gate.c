/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/release_gate.c
 *
 * PURPOSE:
 *   Apply portfolio integrity and ordered product-completion gates while
 *   retaining the explicit prohibition on unverified live trading.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/release_gate.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

static int evidence_accepted(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    const char *feature_id)
{
    return umi_productisation_evidence_effective_state(
        ledger, application_id, UMI_PRODUCTISATION_ASSET_FEATURE,
        feature_id, UMI_PRODUCTISATION_EVIDENCE_DECLARED) ==
        UMI_PRODUCTISATION_EVIDENCE_ACCEPTED;
}

UmiStatus umi_productisation_release_gate_evaluate(
    const UmiProductPortfolioInventory *inventory,
    const UmiProductPortfolioCoverage *coverage,
    const UmiProductisationGapReport *gaps,
    const UmiProductisationCompletionPlan *plan,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationReleaseGate *out_gate)
{
    if (inventory == NULL || coverage == NULL || gaps == NULL ||
        plan == NULL || out_gate == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_gate, 0, sizeof(*out_gate));
    out_gate->application_count = inventory->application_count;
    out_gate->blocker_count = gaps->blocker_count;
    out_gate->warning_count = gaps->high_count + gaps->medium_count +
                              gaps->low_count;
    out_gate->portfolio_coverage_percent = coverage->coverage_percent;
    out_gate->experience_catalogue_valid =
        umi_application_experience_catalogue_validate() == UMI_STATUS_OK;
    out_gate->framework_phase_passed =
        out_gate->experience_catalogue_valid &&
        plan->framework_ready_for_product_adoption;
    out_gate->studio_completion_allowed =
        out_gate->framework_phase_passed &&
        plan->studio_ready_for_completion;
    out_gate->trader_completion_allowed =
        out_gate->framework_phase_passed &&
        plan->trader_ready_for_completion;

    /*
     * Live trading stays prohibited until the broker paper vertical, OMS,
     * independent risk boundary and guarded-live workflow all carry accepted
     * evidence. Application declarations alone never satisfy this gate.
     */
    out_gate->trader_live_execution_allowed =
        out_gate->trader_completion_allowed && ledger != NULL &&
        evidence_accepted(ledger, "org.umicom.trader", "trader.ibkr-paper") &&
        evidence_accepted(ledger, "org.umicom.trader", "trader.oms") &&
        evidence_accepted(ledger, "org.umicom.trader", "trader.risk") &&
        evidence_accepted(ledger, "org.umicom.trader", "trader.live");
    return UMI_STATUS_OK;
}
