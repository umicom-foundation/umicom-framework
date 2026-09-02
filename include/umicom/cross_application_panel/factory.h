/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/factory.h
 *
 * PURPOSE:
 *   Expose the reusable panel factory service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FACTORY_H
#define UMICOM_CROSS_APPLICATION_PANEL_FACTORY_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel factory state data shared with callers of this public contract.
 */
typedef struct UmiPanelFactoryState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelFactoryState;
/**
 * Initialise panel factory state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_factory_state_init(UmiPanelFactoryState *state);
/**
 * Provide the panel factory record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_factory_record(UmiPanelFactoryState *state,UmiStatus status,const char *message);
/**
 * Provide the panel factory healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_factory_healthy(const UmiPanelFactoryState *state);
/**
 * Return the number of records represented by panel factory operation without changing
 * their state.
 */
uint64_t umi_panel_factory_operation_count(const UmiPanelFactoryState *state);
#ifdef __cplusplus
}
#endif
#endif
