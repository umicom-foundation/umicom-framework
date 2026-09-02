/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/types.c
 *
 * PURPOSE:
 *   Implement SDK/runtime state names and bounded text helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/types.h"
#include <string.h>

/*
 * Provide the bounded text length operation used by this module and its client
 * applications.
 */
static size_t bounded_text_length(const char *text, size_t capacity)
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
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}
/*
 * Provide the sdk runtime state text operation used by this module and its client
 * applications.
 */
const char *umi_sdk_runtime_state_text(UmiSdkRuntimeState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_SDK_RUNTIME_STATE_UNKNOWN: return "unknown";
    case UMI_SDK_RUNTIME_STATE_DISCOVERED: return "discovered";
    case UMI_SDK_RUNTIME_STATE_VALIDATED: return "validated";
    case UMI_SDK_RUNTIME_STATE_READY: return "ready";
    case UMI_SDK_RUNTIME_STATE_DEGRADED: return "degraded";
    case UMI_SDK_RUNTIME_STATE_INCOMPATIBLE: return "incompatible";
    case UMI_SDK_RUNTIME_STATE_MISSING: return "missing";
    default: return "invalid";
    }
}
/*
 * Provide the sdk runtime component kind text operation used by this module and its client
 * applications.
 */
const char *umi_sdk_runtime_component_kind_text(UmiSdkRuntimeComponentKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_SDK_RUNTIME_COMPONENT_STATIC_LIBRARY: return "static-library";
    case UMI_SDK_RUNTIME_COMPONENT_SHARED_LIBRARY: return "shared-library";
    case UMI_SDK_RUNTIME_COMPONENT_EXECUTABLE: return "executable";
    case UMI_SDK_RUNTIME_COMPONENT_HEADER: return "header";
    case UMI_SDK_RUNTIME_COMPONENT_RESOURCE: return "resource";
    case UMI_SDK_RUNTIME_COMPONENT_CMAKE_PACKAGE: return "cmake-package";
    case UMI_SDK_RUNTIME_COMPONENT_ADAPTER: return "adapter";
    case UMI_SDK_RUNTIME_COMPONENT_PLUGIN: return "plugin";
    default: return "unknown";
    }
}
/*
 * Provide the sdk runtime copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = bounded_text_length(source, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) { destination[0] = '\0'; return UMI_STATUS_CAPACITY_EXCEEDED; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length != 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
/* Check that sdk runtime text satisfies its contract before another service relies on it. */
bool umi_sdk_runtime_text_is_valid(const char *text, size_t capacity)
{
    return text != NULL && capacity != 0U && bounded_text_length(text, capacity) < capacity;
}
