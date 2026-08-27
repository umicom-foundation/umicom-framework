/*-----------------------------------------------------------------------------
 * Umicom Framework Native Tool
 * File: tools/application_production_audit_main.c
 *
 * PURPOSE:
 *   Emit the authoritative catalogue-to-product gap inventory for every
 *   canonical Umicom application without requiring Python or PowerShell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "umicom/application/production/production.h"

static int structural_capability_probe(const char *capability_id, void *context)
{
    (void)context;
    return capability_id != NULL && capability_id[0] != '\0';
}

int main(void)
{
    UmiApplicationProductionPortfolio *portfolio =
        calloc(1U, sizeof(*portfolio));
    UmiApplicationProductionGapAudit audit;
    size_t application_index;
    UmiStatus status;
    if (portfolio == NULL) {
        (void)fprintf(stderr, "application production audit: allocation failed\n");
        return 2;
    }
    status = umi_application_production_portfolio_build(
        structural_capability_probe, NULL, portfolio);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "application production audit: %s\n",
                      umi_status_text(status));
        free(portfolio);
        return 3;
    }
    status = umi_application_production_gap_audit_build(portfolio, &audit);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "application production audit: %s\n",
                      umi_status_text(status));
        free(portfolio);
        return 4;
    }
    (void)printf(
        "portfolio applications=%zu panels=%zu layouts=%zu features=%zu "
        "framework=%zu application=%zu adapter=%zu planned=%zu "
        "foundation=%zu implemented=%zu verified=%zu uncovered=%zu drift=%zu\n",
        audit.application_count, audit.panel_count, audit.layout_count,
        audit.feature_count, audit.framework_feature_count,
        audit.application_feature_count,
        audit.external_adapter_feature_count, audit.planned_feature_count,
        audit.foundation_feature_count, audit.implemented_feature_count,
        audit.verified_feature_count, audit.uncovered_panel_count,
        audit.manifest_drift_count);
    (void)printf(
        "application_id|panels|layouts|features|planned|foundation|implemented|"
        "verified|application_open|adapter_open|acceptance\n");
    for (application_index = 0U;
         application_index < portfolio->count; ++application_index) {
        const UmiApplicationProductionRuntime *runtime =
            &portfolio->entries[application_index].runtime;
        size_t feature_index;
        size_t planned = 0U;
        size_t foundation = 0U;
        size_t implemented = 0U;
        size_t verified = 0U;
        for (feature_index = 0U; feature_index < runtime->features.count;
             ++feature_index) {
            switch (runtime->features.entries[feature_index].feature->state) {
            case UMI_EXPERIENCE_FEATURE_PLANNED: planned += 1U; break;
            case UMI_EXPERIENCE_FEATURE_FOUNDATION: foundation += 1U; break;
            case UMI_EXPERIENCE_FEATURE_IMPLEMENTED: implemented += 1U; break;
            case UMI_EXPERIENCE_FEATURE_VERIFIED: verified += 1U; break;
            default: break;
            }
        }
        (void)printf("%s|%zu|%zu|%zu|%zu|%zu|%zu|%zu|%zu|%zu|%s\n",
            runtime->binding.experience->application_id,
            runtime->panels.count, runtime->layouts.count,
            runtime->features.count, planned, foundation, implemented,
            verified, runtime->features.application_work_count,
            runtime->features.external_adapter_work_count,
            umi_application_production_state_text(runtime->acceptance.state));
    }
    free(portfolio);
    return 0;
}

