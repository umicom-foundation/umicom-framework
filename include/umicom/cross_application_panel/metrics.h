/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/metrics.h
 *
 * PURPOSE:
 *   Expose the reusable panel metrics service state and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiPanelMetricsState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelMetricsState;
void umi_panel_metrics_state_init(UmiPanelMetricsState *state);
UmiStatus umi_panel_metrics_record(UmiPanelMetricsState *state,UmiStatus status,const char *message);
bool umi_panel_metrics_healthy(const UmiPanelMetricsState *state);
uint64_t umi_panel_metrics_operation_count(const UmiPanelMetricsState *state);
#ifdef __cplusplus
}
#endif
#endif
