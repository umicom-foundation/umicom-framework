/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/release_gate.h
 *
 * PURPOSE:
 *   Evaluate catalogue integrity, Framework-first readiness, product adoption
 *   and guarded Trader release constraints from one evidence snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_RELEASE_GATE_H
#define UMICOM_APPLICATION_PRODUCTISATION_RELEASE_GATE_H

#include "umicom/application/productisation/completion_plan.h"
#include "umicom/application/productisation/coverage.h"
#include "umicom/application/productisation/inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductisationReleaseGate {
    size_t application_count;
    size_t blocker_count;
    size_t warning_count;
    unsigned portfolio_coverage_percent;
    int experience_catalogue_valid;
    int framework_phase_passed;
    int studio_completion_allowed;
    int trader_completion_allowed;
    int trader_live_execution_allowed;
} UmiProductisationReleaseGate;

UmiStatus umi_productisation_release_gate_evaluate(
    const UmiProductPortfolioInventory *inventory,
    const UmiProductPortfolioCoverage *coverage,
    const UmiProductisationGapReport *gaps,
    const UmiProductisationCompletionPlan *plan,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationReleaseGate *out_gate);

#ifdef __cplusplus
}
#endif

#endif
