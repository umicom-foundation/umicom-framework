/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/acceptance_report.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_REPORT_H
#define UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_REPORT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/acceptance_rule.h"
#include "umicom/application/production/evidence_registry.h"
#include "umicom/application/production/manifest_drift.h"
#include "umicom/application/production/readiness_report.h"

/**
 * Represent the application production acceptance report data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionAcceptanceReport {
    UmiApplicationProductionState state;
    size_t requirement_count;
    size_t accepted_count;
    size_t missing_evidence_count;
    uint32_t blocker_flags;
    int accepted;
} UmiApplicationProductionAcceptanceReport;

/**
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
    UmiApplicationProductionAcceptanceReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
