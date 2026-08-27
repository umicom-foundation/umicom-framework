/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_capability_matrix.c
 *
 * PURPOSE:
 *   Verify cross-application capability reuse and component coverage analysis.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/productisation/capability_matrix.h"

int main(void)
{
    UmiProductCapabilityMatrix matrix;
    const UmiProductCapabilityUsage *ui;
    const UmiProductCapabilityUsage *trading;
    assert(umi_product_capability_matrix_build(&matrix) == UMI_STATUS_OK);
    assert(matrix.usage_count > 20U);
    assert(matrix.shared_count > 5U);
    assert(matrix.missing_count == 0U);
    assert(matrix.missing_component_count == 0U);
    ui = umi_product_capability_matrix_find(&matrix, "umicom.ui");
    trading = umi_product_capability_matrix_find(
        &matrix, "umicom.trading");
    assert(ui != NULL && ui->application_count >= 3U);
    assert(ui->panel_reference_count > 0U);
    assert(trading != NULL && trading->component_count > 0U);
    assert(umi_product_capability_usage_has_application(ui, 0U));
    return 0;
}
