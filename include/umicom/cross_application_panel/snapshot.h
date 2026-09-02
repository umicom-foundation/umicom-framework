/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/snapshot.h
 *
 * PURPOSE:
 *   Expose the reusable panel snapshot service state and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_SNAPSHOT_H
#define UMICOM_CROSS_APPLICATION_PANEL_SNAPSHOT_H
#include "umicom/cross_application_panel/definition.h"
#include "umicom/cross_application_panel/instance.h"
#include "umicom/cross_application_panel/context_binding.h"
#include "umicom/cross_application_panel/layout_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel snapshot state data shared with callers of this public contract.
 */
typedef struct UmiPanelSnapshotState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelSnapshotState;
/**
 * Initialise panel snapshot state from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_snapshot_state_init(UmiPanelSnapshotState *state);
/**
 * Provide the panel snapshot record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_snapshot_record(UmiPanelSnapshotState *state,UmiStatus status,const char *message);
/**
 * Provide the panel snapshot healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_snapshot_healthy(const UmiPanelSnapshotState *state);
/**
 * Return the number of records represented by panel snapshot operation without changing
 * their state.
 */
uint64_t umi_panel_snapshot_operation_count(const UmiPanelSnapshotState *state);
#ifdef __cplusplus
}
#endif
#endif
