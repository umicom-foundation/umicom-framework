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
typedef struct UmiFrontendQualityBudget { uint64_t max_transfer_bytes; uint64_t max_script_bytes; uint64_t max_render_us; uint32_t max_console_errors; uint32_t min_accessibility_score; } UmiFrontendQualityBudget;
typedef struct UmiFrontendQualityMetrics { uint64_t transfer_bytes; uint64_t script_bytes; uint64_t render_us; uint32_t console_errors; uint32_t accessibility_score; } UmiFrontendQualityMetrics;
typedef struct UmiFrontendQualityResult { UmiFrontendQualityMetrics metrics; uint32_t failed_checks; int passed; char summary[UMI_FRONTEND_DEV_TEXT_CAPACITY]; } UmiFrontendQualityResult;
UmiFrontendQualityBudget umi_frontend_quality_budget_default(void);
UmiStatus umi_frontend_quality_audit_run(const UmiFrontendQualityBudget *budget,const UmiFrontendQualityMetrics *metrics,UmiFrontendQualityResult *out_result);
#endif
