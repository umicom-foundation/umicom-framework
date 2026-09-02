/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/types.c
 *
 * PURPOSE:
 *   Implement context enumeration text, bounded strings and stable hashing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/types.h"
#include <string.h>

/* Provide the context kind text operation used by this module and its client applications. */
const char *umi_context_kind_text(UmiContextKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_CONTEXT_KIND_GENERIC: return "generic";
    case UMI_CONTEXT_KIND_SOURCE_LOCATION: return "source-location";
    case UMI_CONTEXT_KIND_INSTRUMENT: return "instrument";
    case UMI_CONTEXT_KIND_ACCOUNT: return "account";
    case UMI_CONTEXT_KIND_TRADE: return "trade";
    case UMI_CONTEXT_KIND_PROJECT: return "project";
    case UMI_CONTEXT_KIND_WORKSPACE: return "workspace";
    case UMI_CONTEXT_KIND_MEDIA: return "media";
    case UMI_CONTEXT_KIND_SELECTION: return "selection";
    default: return "unknown";
    
}
}
/*
 * Provide the context colour text operation used by this module and its client
 * applications.
 */
const char *umi_context_colour_text(UmiContextChannelColour colour)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (colour) {
    case UMI_CONTEXT_COLOUR_NONE: return "none";
    case UMI_CONTEXT_COLOUR_RED: return "red";
    case UMI_CONTEXT_COLOUR_ORANGE: return "orange";
    case UMI_CONTEXT_COLOUR_YELLOW: return "yellow";
    case UMI_CONTEXT_COLOUR_GREEN: return "green";
    case UMI_CONTEXT_COLOUR_CYAN: return "cyan";
    case UMI_CONTEXT_COLOUR_BLUE: return "blue";
    case UMI_CONTEXT_COLOUR_PURPLE: return "purple";
    case UMI_CONTEXT_COLOUR_MAGENTA: return "magenta";
    default: return "unknown";
    
}
}
/*
 * Provide the context delivery state text operation used by this module and its client
 * applications.
 */
const char *umi_context_delivery_state_text(UmiContextDeliveryState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_CONTEXT_DELIVERY_CREATED: return "created";
    case UMI_CONTEXT_DELIVERY_ROUTED: return "routed";
    case UMI_CONTEXT_DELIVERY_DELIVERED: return "delivered";
    case UMI_CONTEXT_DELIVERY_REJECTED: return "rejected";
    case UMI_CONTEXT_DELIVERY_DUPLICATE: return "duplicate";
    case UMI_CONTEXT_DELIVERY_EXPIRED: return "expired";
    default: return "unknown";
    
}
}
/*
 * Provide the context controller state text operation used by this module and its client
 * applications.
 */
const char *umi_context_controller_state_text(UmiContextControllerState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_CONTEXT_CONTROLLER_CREATED: return "created";
    case UMI_CONTEXT_CONTROLLER_INITIALISED: return "initialised";
    case UMI_CONTEXT_CONTROLLER_RUNNING: return "running";
    case UMI_CONTEXT_CONTROLLER_QUIESCED: return "quiesced";
    case UMI_CONTEXT_CONTROLLER_STOPPED: return "stopped";
    case UMI_CONTEXT_CONTROLLER_FAILED: return "failed";
    default: return "unknown";
    
}
}
/*
 * Provide the context bounded length operation used by this module and its client
 * applications.
 */
size_t umi_context_bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return capacity;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length < capacity && text[length] != '\0') length += 1U;
    return length;
}
/* Check that context text satisfies its contract before another service relies on it. */
bool umi_context_text_is_valid(const char *text, size_t capacity)
{
    return text != NULL && capacity != 0U &&
           umi_context_bounded_length(text, capacity) < capacity;
}
/* Provide the context copy text operation used by this module and its client applications. */
UmiStatus umi_context_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = umi_context_bounded_length(source, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
destination[0] = '\0';
return UMI_STATUS_CAPACITY_EXCEEDED;
}
    memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
/* Provide the context hash text operation used by this module and its client applications. */
uint64_t umi_context_hash_text(uint64_t hash, const char *text, size_t capacity)
{
    size_t index, length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return hash;
    length = umi_context_bounded_length(text, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return hash;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    
}
    return hash;
}
