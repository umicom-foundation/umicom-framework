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

/*
 * Provide the workbench context host bounded length operation used by this module and its
 * client applications.
 */
size_t umi_workbench_context_host_bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

/*
 * Check that workbench context host text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_host_text_is_valid(const char *text, size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_host_bounded_length(text, capacity) < capacity;
}

/*
 * Provide the workbench context host copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_context_host_bounded_length(source, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_host_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t index;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return hash;
    length = umi_workbench_context_host_bounded_length(text, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the workbench context host kind mask operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_host_kind_mask(UmiContextKind kind)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind < UMI_CONTEXT_KIND_GENERIC || kind > UMI_CONTEXT_KIND_SELECTION) {
        return 0U;
    }
    return UINT64_C(1) << ((unsigned)kind - 1U);
}

/*
 * Provide the workbench context host kind allowed operation used by this module and its
 * client applications.
 */
bool umi_workbench_context_host_kind_allowed(uint64_t mask, UmiContextKind kind)
{
    uint64_t bit = umi_workbench_context_host_kind_mask(kind);
    return bit != 0U && (mask & bit) != 0U;
}

/*
 * Provide the workbench context host panel role text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_host_panel_role_text(
    UmiWorkbenchContextHostPanelRole role)
{
    /* Select the behaviour associated with the requested command or state value. */
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
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_DOCUMENT: return "document";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_DESIGNER: return "designer";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_MEDIA_TIMELINE: return "media-timeline";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_MODEL: return "model";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_KNOWLEDGE: return "knowledge";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_SYSTEM: return "system";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_SERVICE: return "service";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_CUSTOMER: return "customer";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT: return "payment";
    case UMI_WORKBENCH_CONTEXT_HOST_PANEL_TREASURY: return "treasury";
    default: return "unknown";
    }
}

/*
 * Provide the workbench context host endpoint state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_context_host_endpoint_state_text(
    UmiWorkbenchContextHostEndpointState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED: return "disabled";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY: return "ready";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE: return "active";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED: return "suspended";
    case UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ERROR: return "error";
    default: return "unknown";
    }
}

/*
 * Provide the workbench context host delivery disposition text operation used by this
 * module and its client applications.
 */
const char *umi_workbench_context_host_delivery_disposition_text(
    UmiWorkbenchContextHostDeliveryDisposition disposition)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (disposition) {
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_QUEUED: return "queued";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED: return "consumed";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_REJECTED: return "rejected";
    case UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_DROPPED: return "dropped";
    default: return "unknown";
    }
}
