/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_summary.h
 *
 * PURPOSE:
 *   Summarise validation counts into a stable health score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_SUMMARY_H
#define UMICOM_UI_REACTIVE_VALIDATION_SUMMARY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive validation summary data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveValidationSummary {
    size_t valid_count;
    size_t warning_count;
    size_t error_count;
} UmiUiReactiveValidationSummary;
/**
 * Initialise ui reactive validation summary from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_validation_summary_init(UmiUiReactiveValidationSummary *item);
/**
 * Provide the ui reactive validation summary score operation used by this module and its
 * client applications.
 */
unsigned umi_ui_reactive_validation_summary_score(const UmiUiReactiveValidationSummary *item);
#ifdef __cplusplus
}
#endif
#endif
