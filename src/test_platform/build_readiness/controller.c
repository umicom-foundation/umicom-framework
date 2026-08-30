/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/controller.c
 * PURPOSE: Coordinate build probes without embedding OS access in Framework.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/controller.h"

#include <string.h>

#include "umicom/test_platform/build_readiness/catalogue.h"

UmiStatus umi_test_platform_build_readiness_assess_product(
    const char *product_id, UmiTestPlatformBuildProbe probe, void *user_data,
    UmiTestPlatformBuildInventory *inventory,
    UmiTestPlatformBuildReadinessReport *report)
{
    size_t index;
    size_t matched = 0U;
    if (product_id == NULL || product_id[0] == '\0' || probe == NULL ||
        inventory == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_test_platform_build_inventory_init(inventory);
    for (index = 0U; index <
        umi_test_platform_build_artifact_catalogue_count(); ++index) {
        const UmiTestPlatformBuildArtifact *artifact =
            umi_test_platform_build_artifact_catalogue_at(index);
        UmiTestPlatformBuildDiagnosticInput input = {0};
        UmiTestPlatformBuildDiagnostic diagnostic;
        UmiStatus status;
        if (strcmp(artifact->product_id, product_id) != 0) continue;
        matched++;
        status = probe(artifact, &input, user_data);
        if (status != UMI_STATUS_OK) return status;
        status = umi_test_platform_build_diagnostic_classify(&input,
            &diagnostic);
        if (status != UMI_STATUS_OK) return status;
        status = umi_test_platform_build_inventory_record(inventory, artifact,
            diagnostic.state, diagnostic.summary);
        if (status != UMI_STATUS_OK) return status;
    }
    if (matched == 0U) return UMI_STATUS_NOT_FOUND;
    return umi_test_platform_build_readiness_report_create(inventory, report);
}
