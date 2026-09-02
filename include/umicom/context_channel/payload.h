/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/payload.h
 *
 * PURPOSE:
 *   Define immutable typed payloads with canonical domain projections and audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PAYLOAD_H
#define UMICOM_CONTEXT_CHANNEL_PAYLOAD_H
#include "umicom/context_channel/value.h"
#include "umicom/context_channel/source_location.h"
#include "umicom/context_channel/instrument.h"
#include "umicom/context_channel/account.h"
#include "umicom/context_channel/trade.h"
#include "umicom/context_channel/project.h"
#include "umicom/context_channel/workspace.h"
#include "umicom/context_channel/media.h"
#include "umicom/context_channel/selection.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context payload data shared with callers of this public contract.
 */
typedef struct UmiContextPayload {
    uint32_t structure_size;
    UmiContextKind kind;
    UmiContextIdentity identity;
    UmiContextAudit audit;
    UmiContextValue values[UMI_CONTEXT_MAX_VALUES];
    size_t value_count;
    union {
        UmiSourceLocationContext source_location;
        UmiInstrumentContext instrument;
        UmiAccountContext account;
        UmiTradeContext trade;
        UmiProjectContext project;
        UmiWorkspaceContext workspace;
        UmiMediaContext media;
        UmiSelectionContext selection;
    } domain;
    uint64_t content_hash;
} UmiContextPayload;
/**
 * Initialise context payload from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_payload_init(UmiContextPayload *payload,UmiContextKind kind,const char *context_id,const char *schema_id);
/**
 * Check that context payload satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_payload_validate(const UmiContextPayload *payload);
/**
 * Copy context payload into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_context_payload_copy(UmiContextPayload *destination,const UmiContextPayload *source);
/**
 * Provide the context payload add value operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_payload_add_value(UmiContextPayload *payload,const UmiContextValue *value);
/**
 * Provide the context payload find value operation used by this module and its client
 * applications.
 */
const UmiContextValue *umi_context_payload_find_value(const UmiContextPayload *payload,const char *name);
/**
 * Provide the context payload calculate hash operation used by this module and its client
 * applications.
 */
uint64_t umi_context_payload_calculate_hash(const UmiContextPayload *payload);
/**
 * Provide the context payload refresh hash operation used by this module and its client
 * applications.
 */
void umi_context_payload_refresh_hash(UmiContextPayload *payload);
/**
 * Provide the context payload is expired operation used by this module and its client
 * applications.
 */
bool umi_context_payload_is_expired(const UmiContextPayload *payload,uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
