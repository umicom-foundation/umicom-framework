/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_summary.h
 *
 * PURPOSE:
 *   Summarise validation counts into a stable health score.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_SUMMARY_H
#define UMICOM_UI_REACTIVE_VALIDATION_SUMMARY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidationSummary {
    size_t valid_count;
    size_t warning_count;
    size_t error_count;
} UmiUiReactiveValidationSummary;
void umi_ui_reactive_validation_summary_init(UmiUiReactiveValidationSummary *item);
unsigned umi_ui_reactive_validation_summary_score(const UmiUiReactiveValidationSummary *item);
#ifdef __cplusplus
}
#endif
#endif
