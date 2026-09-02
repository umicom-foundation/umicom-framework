/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_gap_plan.c
 *
 * PURPOSE:
 *   Verify shared Framework gaps precede Studio, Trader and other product
 *   adoption steps in the completion plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/application/productisation/completion_plan.h"
#include "umicom/test_runtime/check.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductCapabilityMatrix *matrix =
        (UmiProductCapabilityMatrix *)calloc(1U, sizeof(*matrix));
    UmiProductisationEvidenceLedger *ledger =
        (UmiProductisationEvidenceLedger *)calloc(1U, sizeof(*ledger));
    UmiProductisationGapReport *gaps =
        (UmiProductisationGapReport *)calloc(1U, sizeof(*gaps));
    UmiProductisationCompletionPlan *plan =
        (UmiProductisationCompletionPlan *)calloc(1U, sizeof(*plan));
    size_t index;
    int saw_studio = 0;
    int saw_trader = 0;
    /* These bounded portfolio reports are intentionally large, so the test
     * allocates them away from the native stack and validates every pointer. */
    UMI_TEST_REQUIRE(matrix != NULL && ledger != NULL && gaps != NULL &&
                     plan != NULL);
    umi_productisation_evidence_ledger_init(ledger);
    UMI_TEST_REQUIRE(umi_product_capability_matrix_build(matrix) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_productisation_gap_analysis_run(
        matrix, ledger, gaps) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(gaps->gap_count > 300U && !gaps->capacity_exhausted);
    UMI_TEST_REQUIRE(gaps->framework_gap_count > 0U);
    UMI_TEST_REQUIRE(gaps->application_gap_count > 0U);
    UMI_TEST_REQUIRE(umi_productisation_completion_plan_build(gaps, plan) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(plan->step_count == gaps->gap_count);
    UMI_TEST_REQUIRE(plan->framework_step_count == gaps->framework_gap_count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->step_count; ++index) {
        const UmiProductisationCompletionStep *step =
            umi_productisation_completion_plan_at(plan, index);
        UMI_TEST_REQUIRE(step != NULL);
        /* Apply this branch only when its contract condition is satisfied. */
        if (step->stage == UMI_PRODUCTISATION_STAGE_STUDIO) saw_studio = 1;
        /* Apply this branch only when its contract condition is satisfied. */
        if (step->stage == UMI_PRODUCTISATION_STAGE_TRADER) saw_trader = 1;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U)
            UMI_TEST_REQUIRE(plan->steps[index - 1U].stage <= step->stage);
    }
    UMI_TEST_REQUIRE(saw_studio && saw_trader);
    free(plan);
    free(gaps);
    free(ledger);
    free(matrix);
    return 0;
}
