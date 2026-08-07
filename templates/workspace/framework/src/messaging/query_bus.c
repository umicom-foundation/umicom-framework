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

UmiStatus umi_query_bus_create(UmiQueryBus **out_bus)
{
    if (out_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = calloc(1U, sizeof(UmiQueryBus));
    return *out_bus != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_query_bus_destroy(UmiQueryBus *bus) { free(bus); }

UmiStatus umi_query_bus_register(UmiQueryBus *bus, const char *name,
                                 UmiQueryHandler handler, void *user_data)
{
    size_t index;
    UmiQueryEntry *entry;
    if (bus == 0 || name == 0 || name[0] == '\0' || handler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < bus->count; ++index)
        if (strcmp(bus->entries[index].name, name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (bus->count >= UMI_QUERY_MAX_HANDLERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &bus->entries[bus->count++];
    (void)snprintf(entry->name, sizeof(entry->name), "%s", name);
    entry->handler = handler;
    entry->user_data = user_data;
    return UMI_STATUS_OK;
}

UmiStatus umi_query_bus_execute(UmiQueryBus *bus, const char *name,
                                const char *request, char *response,
                                size_t response_capacity)
{
    size_t index;
    if (bus == 0 || name == 0 || response == 0 || response_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    response[0] = '\0';
    for (index = 0U; index < bus->count; ++index)
        if (strcmp(bus->entries[index].name, name) == 0)
            return bus->entries[index].handler(request != 0 ? request : "",
                                               response, response_capacity,
                                               bus->entries[index].user_data);
    return UMI_STATUS_NOT_FOUND;
}
