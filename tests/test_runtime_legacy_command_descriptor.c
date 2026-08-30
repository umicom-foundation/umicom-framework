/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_legacy_command_descriptor.c
 *
 * PURPOSE:
 *   Preserve compatibility for command descriptors that omit the optional
 *   required-permission string while using the current stable C descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/command_registry.h"

#include <assert.h>
#include <string.h>

static UmiStatus handler(void *user_data,
                         const char *argument,
                         char *out_message,
                         size_t message_capacity)
{
    (void)user_data;
    (void)argument;
    if (out_message != NULL && message_capacity > 0U) {
        out_message[0] = '\0';
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiCommandRegistry *registry = NULL;
    UmiCommandDescriptor descriptor;
    UmiCommandSnapshot snapshot;

    assert(umi_command_registry_create(&registry) == UMI_STATUS_OK);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "legacy.command";
    descriptor.title = "Legacy-compatible command";
    descriptor.category = "Compatibility";
    descriptor.description = "Permission omitted intentionally";
    descriptor.required_permission = NULL;
    descriptor.handler = handler;

    assert(umi_command_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    assert(umi_command_registry_snapshot(registry, descriptor.command_id,
                                         &snapshot) == UMI_STATUS_OK);
    assert(snapshot.required_permission[0] == '\0');
    umi_command_registry_destroy(registry);
    return 0;
}
