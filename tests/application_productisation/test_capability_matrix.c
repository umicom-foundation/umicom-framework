/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_capability_matrix.c
 *
 * PURPOSE:
 *   Verify cross-application capability reuse and component coverage analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "umicom/application/productisation/capability_matrix.h"
#include "umicom/test_runtime/check.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductCapabilityMatrix *matrix =
        (UmiProductCapabilityMatrix *)calloc(1U, sizeof(*matrix));
    const UmiProductCapabilityUsage *ui;
    const UmiProductCapabilityUsage *trading;
    const UmiProductCapabilityUsage *performance;
    const UmiProductCapabilityUsage *options;
    const UmiProductCapabilityUsage *research;
    size_t usage_index;

    UMI_TEST_REQUIRE(matrix != NULL);
    UMI_TEST_REQUIRE(umi_product_capability_matrix_build(matrix) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(matrix->usage_count > 20U);
    UMI_TEST_REQUIRE(matrix->shared_count > 5U);

    /* Print the exact incomplete contracts before the aggregate check. This
     * turns a future catalogue mismatch into an actionable CTest diagnostic. */
    for (usage_index = 0U; usage_index < matrix->usage_count; ++usage_index) {
        const UmiProductCapabilityUsage *usage = &matrix->usages[usage_index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (usage->missing_from_framework) {
            (void)fprintf(stderr, "Missing Framework capability: %s\n",
                          usage->capability_id);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (usage->panel_reference_count > 0U &&
            usage->component_count == 0U) {
            (void)fprintf(stderr, "Missing reusable panel component: %s\n",
                          usage->capability_id);
        }
    }
    UMI_TEST_REQUIRE(matrix->missing_count == 0U);
    UMI_TEST_REQUIRE(matrix->missing_component_count == 0U);
    ui = umi_product_capability_matrix_find(matrix, "umicom.ui");
    trading = umi_product_capability_matrix_find(
        matrix, "umicom.trading");
    performance = umi_product_capability_matrix_find(
        matrix, "umicom.observability.performance");
    options = umi_product_capability_matrix_find(
        matrix, "umicom.trading.options");
    research = umi_product_capability_matrix_find(
        matrix, "umicom.trading.research");
    UMI_TEST_REQUIRE(ui != NULL && ui->application_count >= 3U);
    UMI_TEST_REQUIRE(ui->panel_reference_count > 0U);
    UMI_TEST_REQUIRE(trading != NULL && trading->component_count > 0U);
    UMI_TEST_REQUIRE(umi_product_capability_usage_has_application(ui, 0U));
    /* Newly declared panels must close both capability registration and
     * reusable component coverage before a thin application may adopt them. */
    UMI_TEST_REQUIRE(performance != NULL &&
                     performance->component_count > 0U);
    UMI_TEST_REQUIRE(options != NULL && options->component_count > 0U);
    UMI_TEST_REQUIRE(research != NULL && research->component_count > 0U);
    free(matrix);
    return 0;
}
