/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/acceptance_report.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/acceptance_report.h"

#include <string.h>

enum {
    BLOCK_MANIFEST = 1U << 0,
    BLOCK_LAYOUT = 1U << 1,
    BLOCK_CAPABILITY = 1U << 2,
    BLOCK_TESTS = 1U << 3,
    BLOCK_EVIDENCE = 1U << 4
};

UmiStatus umi_application_production_acceptance_report_evaluate(
    const UmiApplicationProductionAcceptanceRule *rule,
    const UmiApplicationProductionBinding *binding,
    const UmiApplicationProductionReadinessReport *readiness,
    const UmiApplicationProductionManifestDrift *drift,
    const UmiApplicationProductionEvidenceRequirements *requirements,
    const UmiApplicationProductionEvidenceRegistry *evidence,
    UmiApplicationProductionAcceptanceReport *out_report)
{
    size_t index;
    UmiStatus status;
    if (readiness == NULL || drift == NULL || requirements == NULL ||
        evidence == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_acceptance_rule_validate(rule);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_report, 0, sizeof(*out_report));
    if (rule->require_manifest) {
        out_report->requirement_count += 1U;
        if (drift->compatible) out_report->accepted_count += 1U;
        else out_report->blocker_flags |= BLOCK_MANIFEST;
    }
    if (rule->require_layout_projection) {
        out_report->requirement_count += 1U;
        if (readiness->layout_ready) out_report->accepted_count += 1U;
        else out_report->blocker_flags |= BLOCK_LAYOUT;
    }
    if (rule->require_capabilities) {
        out_report->requirement_count += 1U;
        if (umi_application_production_capability_readiness_launchable(
                &readiness->capabilities))
            out_report->accepted_count += 1U;
        else out_report->blocker_flags |= BLOCK_CAPABILITY;
    }
    if (rule->require_tests) {
        out_report->requirement_count += 1U;
        if (readiness->tests_ready) out_report->accepted_count += 1U;
        else out_report->blocker_flags |= BLOCK_TESTS;
    }
    if (rule->require_evidence) {
        for (index = 0U; index < requirements->count; ++index) {
            const UmiApplicationProductionEvidenceRequirement *requirement =
                &requirements->entries[index];
            const UmiApplicationProductionEvidenceRecord *record;
            if (!requirement->required) continue;
            out_report->requirement_count += 1U;
            record = umi_application_production_evidence_registry_find(
                evidence, requirement->evidence_id);
            if (record != NULL &&
                record->state ==
                UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED)
                out_report->accepted_count += 1U;
            else
                out_report->missing_evidence_count += 1U;
        }
        if (out_report->missing_evidence_count > 0U)
            out_report->blocker_flags |= BLOCK_EVIDENCE;
    }
    out_report->accepted = out_report->blocker_flags == 0U;
    if (!out_report->accepted)
        out_report->state = UMI_APPLICATION_PRODUCTION_BLOCKED;
    else if (readiness->state == UMI_APPLICATION_PRODUCTION_DEGRADED)
        out_report->state = UMI_APPLICATION_PRODUCTION_DEGRADED;
    else
        out_report->state = UMI_APPLICATION_PRODUCTION_READY;
    return UMI_STATUS_OK;
}

