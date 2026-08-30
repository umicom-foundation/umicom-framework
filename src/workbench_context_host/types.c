/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/types.c
 *
 * PURPOSE:
 *   Implement bounded text, stable hashing, context-kind masks and enum text conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/types.h"

#include <string.h>

size_t umi_workbench_context_host_bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return 0U;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

bool umi_workbench_context_host_text_is_valid(const char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_host_bounded_length(text, capacity) < capacity;
}

UmiStatus umi_workbench_context_host_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_context_host_bounded_length(source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_host_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t index;
    size_t length;
    if (text == NULL) return hash;
    length = umi_workbench_context_host_bounded_length(text, capacity);
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

uint64_t umi_workbench_context_host_kind_mask(UmiContextKind kind)
{
    if (kind < UMI_CONTEXT_KIND_GENERIC || kind > UMI_CONTEXT_KIND_SELECTION) {
        return 0U;
    }
    return UINT64_C(1) << ((unsigned)kind - 1U);
}

bool umi_workbench_context_host_kind_allowed(uint64_t mask, UmiContextKind kind)
{
    uint64_t bit = umi_workbench_context_host_kind_mask(kind);
    return bit != 0U && (mask & bit) != 0U;
}

const char *umi_workbench_context_host_panel_role_text(
    UmiWorkbenchContextHostPanelRole role)
{
    switch (role) {
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC: return "generic";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER: return "explorer";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR: return "editor";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_PROBLEMS: return "problems";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_SOURCE_CONTROL: return "source-control";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_TEST_EXPLORER: return "test-explorer";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_DEBUGGER: return "debugger";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_TERMINAL: return "terminal";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_OUTPUT: return "output";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_AI: return "ai";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_CHART: return "chart";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_ORDER_ENTRY: return "order-entry";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_RISK: return "risk";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_TRADE: return "trade";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_APPLICATION_LAUNCHER: return "application-launcher";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR: return "context-inspector";
    default: return "unknown";
    }
}

const char *umi_workbench_context_host_endpoint_state_text(
    UmiWorkbenchContextHostEndpointState state)
{
    switch (state) {
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED: return "disabled";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY: return "ready";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE: return "active";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED: return "suspended";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ERROR: return "error";
    default: return "unknown";
    }
}

const char *umi_workbench_context_host_delivery_disposition_text(
    UmiWorkbenchContextHostDeliveryDisposition disposition)
{
    switch (disposition) {
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_QUEUED: return "queued";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED: return "consumed";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_REJECTED: return "rejected";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_DROPPED: return "dropped";
    default: return "unknown";
    }
}
