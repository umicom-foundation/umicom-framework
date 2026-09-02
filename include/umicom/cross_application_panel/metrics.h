/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/metrics.h
 *
 * PURPOSE:
 *   Expose the reusable panel metrics service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_METRICS_H
#define UMICOM_CROSS_APPLICATION_PANEL_METRICS_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel metrics state data shared with callers of this public contract.
 */
typedef struct UmiPanelMetricsState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelMetricsState;
/**
 * Initialise panel metrics state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_metrics_state_init(UmiPanelMetricsState *state);
/**
 * Provide the panel metrics record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_metrics_record(UmiPanelMetricsState *state,UmiStatus status,const char *message);
/**
 * Provide the panel metrics healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_metrics_healthy(const UmiPanelMetricsState *state);
/**
 * Return the number of records represented by panel metrics operation without changing
 * their state.
 */
uint64_t umi_panel_metrics_operation_count(const UmiPanelMetricsState *state);
#ifdef __cplusplus
}
#endif
#endif
