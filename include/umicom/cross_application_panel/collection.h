/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/collection.h
 *
 * PURPOSE:
 *   Expose the reusable panel collection service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_H
#define UMICOM_CROSS_APPLICATION_PANEL_COLLECTION_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel collection state data shared with callers of this public contract.
 */
typedef struct UmiPanelCollectionState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelCollectionState;
/**
 * Initialise panel collection state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_collection_state_init(UmiPanelCollectionState *state);
/**
 * Provide the panel collection record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_collection_record(UmiPanelCollectionState *state,UmiStatus status,const char *message);
/**
 * Provide the panel collection healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_collection_healthy(const UmiPanelCollectionState *state);
/**
 * Return the number of records represented by panel collection operation without changing
 * their state.
 */
uint64_t umi_panel_collection_operation_count(const UmiPanelCollectionState *state);
#ifdef __cplusplus
}
#endif
#endif
