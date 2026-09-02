/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/validation_summary.h
 *
 * PURPOSE:
 *   Aggregate validation markers into release-gate counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_VALIDATION_SUMMARY_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_VALIDATION_SUMMARY_H
#include "umicom/designer/visual_designer/validation_marker.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer validation summary data shared with callers of this public contract.
 */
typedef struct UmiRadValidationSummary { size_t info_count; size_t warning_count; size_t error_count; } UmiRadValidationSummary;
/**
 * Release or reset state held by visual designer validation summary so the same storage can be reused
 * safely.
 */
void umi_rad_validation_summary_clear(UmiRadValidationSummary *summary);
/**
 * Add visual designer validation summary only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_validation_summary_add(UmiRadValidationSummary *summary,const UmiRadValidationMarker *marker);
/**
 * Provide the visual designer validation summary can publish operation used by this module and its
 * client applications.
 */
int umi_rad_validation_summary_can_publish(const UmiRadValidationSummary *summary);
#ifdef __cplusplus
}
#endif
#endif
