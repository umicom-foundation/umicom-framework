/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/transformer_engine.h
 *
 * PURPOSE:
 *   Apply declared schema transformations without leaking product types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TRANSFORMER_ENGINE_H
#define UMICOM_CONTEXT_CHANNEL_TRANSFORMER_ENGINE_H
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
#define UMI_CONTEXT_TRANSFORMER_ENGINE_NAME "transformer-engine"
typedef struct UmiContextTransformerEngineState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextTransformerEngineState;
void umi_context_transformer_engine_state_init(UmiContextTransformerEngineState *state);
UmiStatus umi_context_transformer_engine_state_record(UmiContextTransformerEngineState *state,UmiStatus status,const char *message);
uint64_t umi_context_transformer_engine_operation_count(const UmiContextTransformerEngineState *state);
#ifdef __cplusplus
}
#endif
#endif
