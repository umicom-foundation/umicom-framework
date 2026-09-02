/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/quality_audit.h
 *
 * PURPOSE:
 *   Publish the public quality audit contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_QUALITY_AUDIT_H
#define UMICOM_FRONTEND_QUALITY_AUDIT_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend quality budget data shared with callers of this public contract.
 */
typedef struct UmiFrontendQualityBudget { uint64_t max_transfer_bytes; uint64_t max_script_bytes; uint64_t max_render_us; uint32_t max_console_errors; uint32_t min_accessibility_score; } UmiFrontendQualityBudget;
/**
 * Represent the frontend quality metrics data shared with callers of this public contract.
 */
typedef struct UmiFrontendQualityMetrics { uint64_t transfer_bytes; uint64_t script_bytes; uint64_t render_us; uint32_t console_errors; uint32_t accessibility_score; } UmiFrontendQualityMetrics;
/**
 * Represent the frontend quality result data shared with callers of this public contract.
 */
typedef struct UmiFrontendQualityResult { UmiFrontendQualityMetrics metrics; uint32_t failed_checks; int passed; char summary[UMI_FRONTEND_DEV_TEXT_CAPACITY]; } UmiFrontendQualityResult;
/**
 * Provide the frontend quality budget default operation used by this module and its client
 * applications.
 */
UmiFrontendQualityBudget umi_frontend_quality_budget_default(void);
/**
 * Perform frontend quality audit through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_frontend_quality_audit_run(const UmiFrontendQualityBudget *budget,const UmiFrontendQualityMetrics *metrics,UmiFrontendQualityResult *out_result);
#endif
