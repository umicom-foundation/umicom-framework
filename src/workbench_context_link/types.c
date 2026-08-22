/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/types.c
 *
 * PURPOSE:
 *   Implement bounded text, hashing, context-kind masks and enum text conversion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/types.h"

#include <string.h>

size_t umi_workbench_context_link_bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return 0U;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

bool umi_workbench_context_link_text_is_valid(const char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_link_bounded_length(text, capacity) < capacity;
}

UmiStatus umi_workbench_context_link_copy_text(char *destination,
                                               size_t capacity,
                                               const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_context_link_bounded_length(source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length != 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_link_hash_bytes(uint64_t hash,
                                               const void *bytes,
                                               size_t count)
{
    const unsigned char *input = (const unsigned char *)bytes;
    size_t index;
    if (bytes == NULL && count != 0U) return hash;
    for (index = 0U; index < count; ++index) {
        hash ^= (uint64_t)input[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

uint64_t umi_workbench_context_link_hash_text(uint64_t hash,
                                              const char *text,
                                              size_t capacity)
{
    size_t length;
    if (text == NULL) return hash;
    length = umi_workbench_context_link_bounded_length(text, capacity);
    return umi_workbench_context_link_hash_bytes(hash, text, length);
}

uint64_t umi_workbench_context_link_kind_mask(UmiContextKind kind)
{
    if (kind < UMI_CONTEXT_KIND_GENERIC || kind > UMI_CONTEXT_KIND_SELECTION) return 0U;
    return UINT64_C(1) << ((unsigned)kind - 1U);
}

bool umi_workbench_context_link_kind_allowed(uint64_t mask, UmiContextKind kind)
{
    const uint64_t bit = umi_workbench_context_link_kind_mask(kind);
    return bit != 0U && (mask & bit) != 0U;
}

const char *umi_workbench_context_link_mode_text(UmiWorkbenchContextLinkMode mode)
{
    switch (mode) {
    case UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE: return "none";
    case UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW: return "follow";
    case UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH: return "publish";
    case UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL: return "bidirectional";
    default: return "unknown";
    }
}

const char *umi_workbench_context_link_state_text(UmiWorkbenchContextLinkState state)
{
    switch (state) {
    case UMI_WORKBENCH_CONTEXT_LINK_STATE_DETACHED: return "detached";
    case UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED: return "attached";
    case UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE: return "active";
    case UMI_WORKBENCH_CONTEXT_LINK_STATE_SUSPENDED: return "suspended";
    case UMI_WORKBENCH_CONTEXT_LINK_STATE_CONFLICT: return "conflict";
    default: return "unknown";
    }
}

const char *umi_workbench_context_link_origin_text(UmiWorkbenchContextLinkOrigin origin)
{
    switch (origin) {
    case UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_USER: return "user";
    case UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_LAYOUT: return "layout";
    case UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_SESSION: return "session";
    case UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_APPLICATION: return "application";
    default: return "unknown";
    }
}

const char *umi_workbench_context_link_priority_text(UmiWorkbenchContextLinkPriority priority)
{
    switch (priority) {
    case UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_LOW: return "low";
    case UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_NORMAL: return "normal";
    case UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_HIGH: return "high";
    case UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_CRITICAL: return "critical";
    default: return "unknown";
    }
}
