/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/group_service.h
 *
 * PURPOSE:
 *   Manage typed colour groups and membership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_GROUP_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_GROUP_SERVICE_H
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
#define UMI_CONTEXT_GROUP_SERVICE_NAME "group-service"
/**
 * Represent the context group service state data shared with callers of this public
 * contract.
 */
typedef struct UmiContextGroupServiceState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextGroupServiceState;
/**
 * Initialise context group service state from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_group_service_state_init(UmiContextGroupServiceState *state);
/**
 * Provide the context group service state record operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_group_service_state_record(UmiContextGroupServiceState *state,UmiStatus status,const char *message);
/**
 * Return the number of records represented by context group service operation without
 * changing their state.
 */
uint64_t umi_context_group_service_operation_count(const UmiContextGroupServiceState *state);
#ifdef __cplusplus
}
#endif
#endif
