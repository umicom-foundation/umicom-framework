/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/audit.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_AUDIT_H
#define UMICOM_APPLICATION_PRODUCTION_AUDIT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/portfolio_report.h"

/**
 * Represent the application production gap audit data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationProductionGapAudit {
    size_t application_count;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
    size_t framework_feature_count;
    size_t application_feature_count;
    size_t external_adapter_feature_count;
    size_t planned_feature_count;
    size_t foundation_feature_count;
    size_t implemented_feature_count;
    size_t verified_feature_count;
    size_t uncovered_panel_count;
    size_t manifest_drift_count;
} UmiApplicationProductionGapAudit;

/**
 * Provide the application production gap audit build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_production_gap_audit_build(
    const UmiApplicationProductionPortfolio *portfolio,
    UmiApplicationProductionGapAudit *out_audit);

#ifdef __cplusplus
}
#endif
#endif
