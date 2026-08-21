/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/persistence.h
 *
 * PURPOSE:
 *   Expose the reusable panel persistence service state and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiPanelPersistenceState { uint64_t revision; uint64_t operation_count; UmiStatus last_status; char last_message[UMI_PANEL_DESCRIPTION_CAPACITY]; } UmiPanelPersistenceState;
void umi_panel_persistence_state_init(UmiPanelPersistenceState *state);
UmiStatus umi_panel_persistence_record(UmiPanelPersistenceState *state,UmiStatus status,const char *message);
bool umi_panel_persistence_healthy(const UmiPanelPersistenceState *state);
uint64_t umi_panel_persistence_operation_count(const UmiPanelPersistenceState *state);
#ifdef __cplusplus
}
#endif
#endif
