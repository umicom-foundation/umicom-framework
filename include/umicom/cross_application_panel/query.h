/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/query.h
 *
 * PURPOSE:
 *   Expose the reusable panel query service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_QUERY_H
#define UMICOM_CROSS_APPLICATION_PANEL_QUERY_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel query state data shared with callers of this public contract.
 */
typedef struct UmiPanelQueryState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelQueryState;
/**
 * Initialise panel query state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_query_state_init(UmiPanelQueryState *state);
/**
 * Provide the panel query record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_query_record(UmiPanelQueryState *state,UmiStatus status,const char *message);
/**
 * Provide the panel query healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_query_healthy(const UmiPanelQueryState *state);
/**
 * Return the number of records represented by panel query operation without changing their
 * state.
 */
uint64_t umi_panel_query_operation_count(const UmiPanelQueryState *state);
#ifdef __cplusplus
}
#endif
#endif
