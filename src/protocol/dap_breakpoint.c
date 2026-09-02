/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_breakpoint.c
 *
 * PURPOSE:
 *   Implement bounded breakpoint storage and DAP setBreakpoints request generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/dap_breakpoint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/protocol/json.h"

struct UmiDapBreakpointRegistry {
    UmiDapBreakpoint items[UMI_PROTOCOL_MAX_BREAKPOINTS];
    size_t count;
};

/*
 * Initialise dap breakpoint registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_dap_breakpoint_registry_create(
    UmiDapBreakpointRegistry **out_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = calloc(1U, sizeof(UmiDapBreakpointRegistry));
    return *out_registry != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by dap breakpoint registry so the same storage can be reused
 * safely.
 */
void umi_dap_breakpoint_registry_destroy(
    UmiDapBreakpointRegistry *registry)
{
    free(registry);
}

/* Add dap breakpoint only after its inputs and available capacity have been checked. */
UmiStatus umi_dap_breakpoint_add(UmiDapBreakpointRegistry *registry,
                                 const UmiDapBreakpoint *breakpoint)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || breakpoint == NULL ||
        breakpoint->source_path[0] == '\0' || breakpoint->line <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].source_path,
                   breakpoint->source_path) == 0 &&
            registry->items[index].line == breakpoint->line) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_PROTOCOL_MAX_BREAKPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->items[registry->count++] = *breakpoint;
    return UMI_STATUS_OK;
}

/*
 * Remove dap breakpoint while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_dap_breakpoint_remove(UmiDapBreakpointRegistry *registry,
                                    const char *source_path,
                                    int line)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || source_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].source_path,
                   source_path) == 0 &&
            registry->items[index].line == line) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < registry->count) {
                (void)memmove(&registry->items[index],
                              &registry->items[index + 1U],
                              (registry->count - index - 1U) *
                                  sizeof(registry->items[0]));
            }
            registry->count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Return the number of records represented by dap breakpoint without changing their state. */
size_t umi_dap_breakpoint_count(const UmiDapBreakpointRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Find dap breakpoint while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDapBreakpoint *umi_dap_breakpoint_at(
    const UmiDapBreakpointRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? &registry->items[index]
        : NULL;
}

/*
 * Provide the dap breakpoint build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_dap_breakpoint_build_request(
    const UmiDapBreakpointRegistry *registry,
    UmiProtocolClient *client,
    const char *source_path,
    int64_t *out_request_id)
{
    char escaped_path[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    size_t used;
    size_t index;
    int written;
    int first = 1;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || client == NULL || source_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(source_path,
                             escaped_path,
                             sizeof(escaped_path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    written = snprintf(params,
                       sizeof(params),
                       "{\"source\":{\"path\":\"%s\"},\"breakpoints\":[",
                       escaped_path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(params)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiDapBreakpoint *breakpoint = &registry->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (!breakpoint->enabled ||
            strcmp(breakpoint->source_path, source_path) != 0) {
            continue;
        }
        written = snprintf(params + used,
                           sizeof(params) - used,
                           "%s{\"line\":%d,\"column\":%d}",
                           first ? "" : ",",
                           breakpoint->line,
                           breakpoint->column > 0 ? breakpoint->column : 1);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(params) - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
        first = 0;
    }
    written = snprintf(params + used,
                       sizeof(params) - used,
                       "]}");
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(params) - used) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_protocol_client_request(client,
                                       "setBreakpoints",
                                       params,
                                       out_request_id);
}
