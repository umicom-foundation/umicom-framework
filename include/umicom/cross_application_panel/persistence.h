/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/persistence.h
 *
 * PURPOSE:
 *   Expose the reusable panel persistence service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_PERSISTENCE_H
#define UMICOM_CROSS_APPLICATION_PANEL_PERSISTENCE_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel persistence state data shared with callers of this public contract.
 */
typedef struct UmiPanelPersistenceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelPersistenceState;
/**
 * Initialise panel persistence state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_persistence_state_init(UmiPanelPersistenceState *state);
/**
 * Provide the panel persistence record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_persistence_record(UmiPanelPersistenceState *state,UmiStatus status,const char *message);
/**
 * Provide the panel persistence healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_persistence_healthy(const UmiPanelPersistenceState *state);
/**
 * Return the number of records represented by panel persistence operation without changing
 * their state.
 */
uint64_t umi_panel_persistence_operation_count(const UmiPanelPersistenceState *state);
#ifdef __cplusplus
}
#endif
#endif
