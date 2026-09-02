/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/messaging/command_bus.c
 *
 * PURPOSE:
 *   Implement the command bus behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/command_bus.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UMI_COMMAND_MAX_HANDLERS 128U

typedef struct UmiCommandEntry {
    char name[96];
    UmiCommandHandler handler;
    void *user_data;
} UmiCommandEntry;

struct UmiCommandBus {
    UmiCommandEntry entries[UMI_COMMAND_MAX_HANDLERS];
    size_t count;
};

/*
 * Initialise command bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_command_bus_create(UmiCommandBus **out_bus)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = calloc(1U, sizeof(UmiCommandBus));
    return *out_bus != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/* Release or reset state held by command bus so the same storage can be reused safely. */
void umi_command_bus_destroy(UmiCommandBus *bus) { free(bus); }

/* Add command bus only after its inputs and available capacity have been checked. */
UmiStatus umi_command_bus_register(UmiCommandBus *bus, const char *name,
                                   UmiCommandHandler handler, void *user_data)
{
    size_t index;
    UmiCommandEntry *entry;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bus == 0 || name == 0 || name[0] == '\0' || handler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->entries[index].name, name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bus->count >= UMI_COMMAND_MAX_HANDLERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &bus->entries[bus->count++];
    (void)snprintf(entry->name, sizeof(entry->name), "%s", name);
    entry->handler = handler;
    entry->user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Perform command bus through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_command_bus_execute(UmiCommandBus *bus, const char *name,
                                  const char *payload)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (bus == 0 || name == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->entries[index].name, name) == 0)
            return bus->entries[index].handler(payload != 0 ? payload : "",
                                               bus->entries[index].user_data);
    return UMI_STATUS_NOT_FOUND;
}
