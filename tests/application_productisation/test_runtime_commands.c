/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_runtime_commands.c
 *
 * PURPOSE:
 *   Verify runtime refresh after evidence changes and the stable command
 *   surface reserved for thin Studio and Desk contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "umicom/application/productisation/command_catalogue.h"
#include "umicom/application/productisation/runtime.h"

int main(void)
{
    UmiProductisationRuntime *runtime = NULL;
    unsigned before;
    assert(umi_productisation_runtime_create(&runtime) == UMI_STATUS_OK);
    assert(runtime != NULL);
    before = runtime->coverage.coverage_percent;
    assert(umi_productisation_runtime_record_evidence(
        runtime, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_LAYOUT,
        "development", "gtk", UMI_PRODUCTISATION_EVIDENCE_ACCEPTED,
        "studio.layout.development") == UMI_STATUS_OK);
    assert(runtime->revision == 2U);
    assert(runtime->coverage.coverage_percent >= before);
    assert(umi_productisation_command_catalogue_count() == 13U);
    assert(umi_productisation_command_catalogue_find(
        "umicom.productisation.openCapabilityMatrix") != NULL);
    assert(umi_productisation_command_catalogue_find("unknown") == NULL);
    umi_productisation_runtime_destroy(runtime);
    return 0;
}
