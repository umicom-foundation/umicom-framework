/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/history_service.h
 *
 * PURPOSE:
 *   Search and prune bounded context history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_HISTORY_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_HISTORY_SERVICE_H
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
#define UMI_CONTEXT_HISTORY_SERVICE_NAME "history-service"
typedef struct UmiContextHistoryServiceState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextHistoryServiceState;
void umi_context_history_service_state_init(UmiContextHistoryServiceState *state);
UmiStatus umi_context_history_service_state_record(UmiContextHistoryServiceState *state,UmiStatus status,const char *message);
uint64_t umi_context_history_service_operation_count(const UmiContextHistoryServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
