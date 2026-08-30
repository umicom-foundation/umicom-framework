/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/types.h
 *
 * PURPOSE:
 *   Define stable C23 value types for typed cross-application context channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TYPES_H
#define UMICOM_CONTEXT_CHANNEL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CONTEXT_TEXT_CAPACITY 160U
#define UMI_CONTEXT_PATH_CAPACITY 512U
#define UMI_CONTEXT_VALUE_CAPACITY 512U
#define UMI_CONTEXT_MAX_VALUES 16U
#define UMI_CONTEXT_MAX_CHANNELS 32U
#define UMI_CONTEXT_MAX_SCHEMAS 64U
#define UMI_CONTEXT_MAX_SUBSCRIPTIONS 256U
#define UMI_CONTEXT_MAX_ROUTES 128U
#define UMI_CONTEXT_MAX_RECENT_IDS 128U
#define UMI_CONTEXT_MAX_LISTENERS 64U
#define UMI_CONTEXT_MAX_DELIVERIES 256U

typedef enum UmiContextKind {
    UMI_CONTEXT_KIND_GENERIC = 1,
    UMI_CONTEXT_KIND_SOURCE_LOCATION = 2,
    UMI_CONTEXT_KIND_INSTRUMENT = 3,
    UMI_CONTEXT_KIND_ACCOUNT = 4,
    UMI_CONTEXT_KIND_TRADE = 5,
    UMI_CONTEXT_KIND_PROJECT = 6,
    UMI_CONTEXT_KIND_WORKSPACE = 7,
    UMI_CONTEXT_KIND_MEDIA = 8,
    UMI_CONTEXT_KIND_SELECTION = 9
} UmiContextKind;

typedef enum UmiContextValueKind {
    UMI_CONTEXT_VALUE_NONE = 0,
    UMI_CONTEXT_VALUE_TEXT = 1,
    UMI_CONTEXT_VALUE_INTEGER = 2,
    UMI_CONTEXT_VALUE_UNSIGNED = 3,
    UMI_CONTEXT_VALUE_DECIMAL = 4,
    UMI_CONTEXT_VALUE_BOOLEAN = 5
} UmiContextValueKind;

typedef enum UmiContextChannelColour {
    UMI_CONTEXT_COLOUR_NONE = 0,
    UMI_CONTEXT_COLOUR_RED = 1,
    UMI_CONTEXT_COLOUR_ORANGE = 2,
    UMI_CONTEXT_COLOUR_YELLOW = 3,
    UMI_CONTEXT_COLOUR_GREEN = 4,
    UMI_CONTEXT_COLOUR_CYAN = 5,
    UMI_CONTEXT_COLOUR_BLUE = 6,
    UMI_CONTEXT_COLOUR_PURPLE = 7,
    UMI_CONTEXT_COLOUR_MAGENTA = 8
} UmiContextChannelColour;

typedef enum UmiContextSubscriptionRole {
    UMI_CONTEXT_ROLE_OBSERVER = 1,
    UMI_CONTEXT_ROLE_PUBLISHER = 2,
    UMI_CONTEXT_ROLE_BIDIRECTIONAL = 3
} UmiContextSubscriptionRole;

typedef enum UmiContextDeliveryState {
    UMI_CONTEXT_DELIVERY_CREATED = 1,
    UMI_CONTEXT_DELIVERY_ROUTED = 2,
    UMI_CONTEXT_DELIVERY_DELIVERED = 3,
    UMI_CONTEXT_DELIVERY_REJECTED = 4,
    UMI_CONTEXT_DELIVERY_DUPLICATE = 5,
    UMI_CONTEXT_DELIVERY_EXPIRED = 6
} UmiContextDeliveryState;

typedef enum UmiContextPolicyDecision {
    UMI_CONTEXT_POLICY_ALLOW = 1,
    UMI_CONTEXT_POLICY_DENY = 2,
    UMI_CONTEXT_POLICY_REDACT = 3
} UmiContextPolicyDecision;

typedef enum UmiContextControllerState {
    UMI_CONTEXT_CONTROLLER_CREATED = 1,
    UMI_CONTEXT_CONTROLLER_INITIALISED = 2,
    UMI_CONTEXT_CONTROLLER_RUNNING = 3,
    UMI_CONTEXT_CONTROLLER_QUIESCED = 4,
    UMI_CONTEXT_CONTROLLER_STOPPED = 5,
    UMI_CONTEXT_CONTROLLER_FAILED = 6
} UmiContextControllerState;

typedef struct UmiContextIdentity {
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char correlation_id[UMI_CONTEXT_TEXT_CAPACITY];
    char causation_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t created_at_ms;
} UmiContextIdentity;

typedef struct UmiContextAudit {
    char actor_id[UMI_CONTEXT_TEXT_CAPACITY];
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t published_at_ms;
    uint64_t expires_at_ms;
    uint64_t revision;
} UmiContextAudit;

const char *umi_context_kind_text(UmiContextKind kind);
const char *umi_context_colour_text(UmiContextChannelColour colour);
const char *umi_context_delivery_state_text(UmiContextDeliveryState state);
const char *umi_context_controller_state_text(UmiContextControllerState state);
size_t umi_context_bounded_length(const char *text, size_t capacity);
bool umi_context_text_is_valid(const char *text, size_t capacity);
UmiStatus umi_context_copy_text(char *destination, size_t capacity, const char *source);
uint64_t umi_context_hash_text(uint64_t hash, const char *text, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
