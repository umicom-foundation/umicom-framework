/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/validation_summary.h
 *
 * PURPOSE:
 *   Aggregate validation markers into release-gate counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_VALIDATION_SUMMARY_H
#define UMICOM_DESIGNER_RAD_VALIDATION_SUMMARY_H
#include "umicom/designer/rad/validation_marker.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadValidationSummary { size_t info_count; size_t warning_count; size_t error_count; } UmiRadValidationSummary;
void umi_rad_validation_summary_clear(UmiRadValidationSummary *summary);
UmiStatus umi_rad_validation_summary_add(UmiRadValidationSummary *summary,const UmiRadValidationMarker *marker);
int umi_rad_validation_summary_can_publish(const UmiRadValidationSummary *summary);
#ifdef __cplusplus
}
#endif
#endif
