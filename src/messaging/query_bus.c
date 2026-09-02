/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/query_bus.c
 *
 * PURPOSE:
 *   Implement the query bus behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/query_bus.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UMI_QUERY_MAX_HANDLERS 128U

typedef struct UmiQueryEntry {
    char name[96];
    UmiQueryHandler handler;
    void *user_data;
} UmiQueryEntry;

struct UmiQueryBus {
    UmiQueryEntry entries[UMI_QUERY_MAX_HANDLERS];
    size_t count;
};

/*
 * Initialise query bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_query_bus_create(UmiQueryBus **out_bus)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = calloc(1U, sizeof(UmiQueryBus));
    return *out_bus != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/* Release or reset state held by query bus so the same storage can be reused safely. */
void umi_query_bus_destroy(UmiQueryBus *bus) { free(bus); }

/* Add query bus only after its inputs and available capacity have been checked. */
UmiStatus umi_query_bus_register(UmiQueryBus *bus, const char *name,
                                 UmiQueryHandler handler, void *user_data)
{
    size_t index;
    UmiQueryEntry *entry;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bus == 0 || name == 0 || name[0] == '\0' || handler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->entries[index].name, name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bus->count >= UMI_QUERY_MAX_HANDLERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &bus->entries[bus->count++];
    (void)snprintf(entry->name, sizeof(entry->name), "%s", name);
    entry->handler = handler;
    entry->user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Perform query bus through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_query_bus_execute(UmiQueryBus *bus, const char *name,
                                const char *request, char *response,
                                size_t response_capacity)
{
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bus == 0 || name == 0 || response == 0 || response_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    response[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->entries[index].name, name) == 0)
            return bus->entries[index].handler(request != 0 ? request : "",
                                               response, response_capacity,
                                               bus->entries[index].user_data);
    return UMI_STATUS_NOT_FOUND;
}
