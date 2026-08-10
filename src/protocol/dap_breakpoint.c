/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_breakpoint.c
 *
 * PURPOSE:
 *   Implement bounded breakpoint storage and DAP setBreakpoints request generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_dap_breakpoint_registry_create(
    UmiDapBreakpointRegistry **out_registry)
{
    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = calloc(1U, sizeof(UmiDapBreakpointRegistry));
    return *out_registry != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_dap_breakpoint_registry_destroy(
    UmiDapBreakpointRegistry *registry)
{
    free(registry);
}

UmiStatus umi_dap_breakpoint_add(UmiDapBreakpointRegistry *registry,
                                 const UmiDapBreakpoint *breakpoint)
{
    size_t index;
    if (registry == NULL || breakpoint == NULL ||
        breakpoint->source_path[0] == '\0' || breakpoint->line <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].source_path,
                   breakpoint->source_path) == 0 &&
            registry->items[index].line == breakpoint->line) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_PROTOCOL_MAX_BREAKPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->items[registry->count++] = *breakpoint;
    return UMI_STATUS_OK;
}

UmiStatus umi_dap_breakpoint_remove(UmiDapBreakpointRegistry *registry,
                                    const char *source_path,
                                    int line)
{
    size_t index;
    if (registry == NULL || source_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].source_path,
                   source_path) == 0 &&
            registry->items[index].line == line) {
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

size_t umi_dap_breakpoint_count(const UmiDapBreakpointRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

const UmiDapBreakpoint *umi_dap_breakpoint_at(
    const UmiDapBreakpointRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? &registry->items[index]
        : NULL;
}

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
    if (registry == NULL || client == NULL || source_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(source_path,
                             escaped_path,
                             sizeof(escaped_path));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    written = snprintf(params,
                       sizeof(params),
                       "{\"source\":{\"path\":\"%s\"},\"breakpoints\":[",
                       escaped_path);
    if (written < 0 || (size_t)written >= sizeof(params)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;
    for (index = 0U; index < registry->count; ++index) {
        const UmiDapBreakpoint *breakpoint = &registry->items[index];
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
        if (written < 0 || (size_t)written >= sizeof(params) - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
        first = 0;
    }
    written = snprintf(params + used,
                       sizeof(params) - used,
                       "]}");
    if (written < 0 || (size_t)written >= sizeof(params) - used) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_protocol_client_request(client,
                                       "setBreakpoints",
                                       params,
                                       out_request_id);
}
