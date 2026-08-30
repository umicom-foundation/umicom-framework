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

static size_t bounded_text_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return capacity;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}
const char *umi_sdk_runtime_state_text(UmiSdkRuntimeState state)
{
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
const char *umi_sdk_runtime_component_kind_text(UmiSdkRuntimeComponentKind kind)
{
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
UmiStatus umi_sdk_runtime_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = bounded_text_length(source, capacity);
    if (length >= capacity) { destination[0] = '\0'; return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (length != 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
bool umi_sdk_runtime_text_is_valid(const char *text, size_t capacity)
{
    return text != NULL && capacity != 0U && bounded_text_length(text, capacity) < capacity;
}
