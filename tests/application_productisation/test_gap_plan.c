/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_gap_plan.c
 *
 * PURPOSE:
 *   Verify shared Framework gaps precede Studio, Trader and other product
 *   adoption steps in the completion plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/productisation/completion_plan.h"

int main(void)
{
    UmiProductCapabilityMatrix matrix;
    UmiProductisationEvidenceLedger ledger;
    UmiProductisationGapReport gaps;
    UmiProductisationCompletionPlan plan;
    size_t index;
    int saw_studio = 0;
    int saw_trader = 0;
    umi_productisation_evidence_ledger_init(&ledger);
    assert(umi_product_capability_matrix_build(&matrix) == UMI_STATUS_OK);
    assert(umi_productisation_gap_analysis_run(
        &matrix, &ledger, &gaps) == UMI_STATUS_OK);
    assert(gaps.gap_count > 300U && !gaps.capacity_exhausted);
    assert(gaps.framework_gap_count > 0U);
    assert(gaps.application_gap_count > 0U);
    assert(umi_productisation_completion_plan_build(&gaps, &plan) ==
           UMI_STATUS_OK);
    assert(plan.step_count == gaps.gap_count);
    assert(plan.framework_step_count == gaps.framework_gap_count);
    for (index = 0U; index < plan.step_count; ++index) {
        const UmiProductisationCompletionStep *step =
            umi_productisation_completion_plan_at(&plan, index);
        assert(step != NULL);
        if (step->stage == UMI_PRODUCTISATION_STAGE_STUDIO) saw_studio = 1;
        if (step->stage == UMI_PRODUCTISATION_STAGE_TRADER) saw_trader = 1;
        if (index > 0U)
            assert(plan.steps[index - 1U].stage <= step->stage);
    }
    assert(saw_studio && saw_trader);
    return 0;
}
