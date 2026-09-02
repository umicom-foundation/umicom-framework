/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/acceptance_report.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Provide the application production acceptance report evaluate operation used by this
 * module and its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (readiness == NULL || drift == NULL || requirements == NULL ||
        evidence == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_acceptance_rule_validate(rule);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_binding_validate(binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_report, 0, sizeof(*out_report));
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->require_manifest) {
        out_report->requirement_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (drift->compatible) out_report->accepted_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_report->blocker_flags |= BLOCK_MANIFEST;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->require_layout_projection) {
        out_report->requirement_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (readiness->layout_ready) out_report->accepted_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_report->blocker_flags |= BLOCK_LAYOUT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->require_capabilities) {
        out_report->requirement_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_production_capability_readiness_launchable(
                &readiness->capabilities))
            out_report->accepted_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_report->blocker_flags |= BLOCK_CAPABILITY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->require_tests) {
        out_report->requirement_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (readiness->tests_ready) out_report->accepted_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_report->blocker_flags |= BLOCK_TESTS;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->require_evidence) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < requirements->count; ++index) {
            const UmiApplicationProductionEvidenceRequirement *requirement =
                &requirements->entries[index];
            const UmiApplicationProductionEvidenceRecord *record;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!requirement->required) continue;
            out_report->requirement_count += 1U;
            record = umi_application_production_evidence_registry_find(
                evidence, requirement->evidence_id);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (record != NULL &&
                record->state ==
                UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED)
                out_report->accepted_count += 1U;
            /* Use this fallback path when the earlier condition does not apply. */
            else
                out_report->missing_evidence_count += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (out_report->missing_evidence_count > 0U)
            out_report->blocker_flags |= BLOCK_EVIDENCE;
    }
    out_report->accepted = out_report->blocker_flags == 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!out_report->accepted)
        out_report->state = UMI_APPLICATION_PRODUCTION_BLOCKED;
    else /* Apply this branch only when its contract condition is satisfied. */ if (readiness->state == UMI_APPLICATION_PRODUCTION_DEGRADED)
        out_report->state = UMI_APPLICATION_PRODUCTION_DEGRADED;
    /* Use this fallback path when the earlier condition does not apply. */
    else
        out_report->state = UMI_APPLICATION_PRODUCTION_READY;
    return UMI_STATUS_OK;
}

