/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/diagnostics_service.h
 *
 * PURPOSE:
 *   Aggregate context routing and delivery diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_DIAGNOSTICS_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_DIAGNOSTICS_SERVICE_H
#include "umicom/context_channel/registry.h"
#include "umicom/context_channel/router.h"
#include "umicom/context_channel/metrics.h"
#include "umicom/context_channel/panel.h"
#include "umicom/context_channel/panel_instance.h"
#include "umicom/context_channel/group_member.h"
#include "umicom/context_channel/link_group.h"
#include "umicom/context_channel/history_entry.h"
#include "umicom/context_channel/diagnostic.h"
#include "umicom/context_channel/bookmark.h"
#include "umicom/context_channel/session.h"
#include "umicom/context_channel/capability.h"
#include "umicom/context_channel/filter_rule.h"
#include "umicom/context_channel/transformer_rule.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_DIAGNOSTICS_SERVICE_NAME "diagnostics-service"
/**
 * Represent the context diagnostics service state data shared with callers of this public
 * contract.
 */
typedef struct UmiContextDiagnosticsServiceState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextDiagnosticsServiceState;
/**
 * Initialise context diagnostics service state from caller-provided values so later
 * operations receive a known state.
 */
void umi_context_diagnostics_service_state_init(UmiContextDiagnosticsServiceState *state);
/**
 * Provide the context diagnostics service state record operation used by this module and
 * its client applications.
 */
UmiStatus umi_context_diagnostics_service_state_record(UmiContextDiagnosticsServiceState *state,UmiStatus status,const char *message);
/**
 * Return the number of records represented by context diagnostics service operation
 * without changing their state.
 */
uint64_t umi_context_diagnostics_service_operation_count(const UmiContextDiagnosticsServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
