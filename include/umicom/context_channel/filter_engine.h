/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/filter_engine.h
 *
 * PURPOSE:
 *   Apply declared field filters before routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_FILTER_ENGINE_H
#define UMICOM_CONTEXT_CHANNEL_FILTER_ENGINE_H
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
#define UMI_CONTEXT_FILTER_ENGINE_NAME "filter-engine"
typedef struct UmiContextFilterEngineState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextFilterEngineState;
void umi_context_filter_engine_state_init(UmiContextFilterEngineState *state);
UmiStatus umi_context_filter_engine_state_record(UmiContextFilterEngineState *state,UmiStatus status,const char *message);
uint64_t umi_context_filter_engine_operation_count(const UmiContextFilterEngineState *state);
#ifdef __cplusplus
}
#endif
#endif
