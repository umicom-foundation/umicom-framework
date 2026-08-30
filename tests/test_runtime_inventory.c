/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_inventory.c
 *
 * PURPOSE:
 *   Verify that runtime inventory snapshots report command, service and
 *   capability registry contents and flag classifications accurately.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/inventory.h"

#include <assert.h>
#include <string.h>

static UmiStatus noop_command(void *user_data,
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
    UmiCommandRegistry *commands = NULL;
    UmiServiceRegistry *services = NULL;
    UmiCapabilityRegistry *capabilities = NULL;
    UmiRuntimeInventorySnapshot snapshot;
    UmiCommandDescriptor command;
    UmiServiceDescriptor service;
    UmiCapabilityDescriptor capability;
    int service_value = 7;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_service_registry_create(&services) == UMI_STATUS_OK);
    assert(umi_capability_registry_create(&capabilities) == UMI_STATUS_OK);

    (void)memset(&command, 0, sizeof(command));
    command.structure_size = (uint32_t)sizeof(command);
    command.command_id = "test.inventory.command";
    command.title = "Inventory Command";
    command.category = "Tests";
    command.description = "Inventory classification command";
    command.required_permission = "";
    command.flags = UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED;
    command.handler = noop_command;
    assert(umi_command_registry_register(commands, &command) == UMI_STATUS_OK);

    (void)memset(&service, 0, sizeof(service));
    service.structure_size = (uint32_t)sizeof(service);
    service.service_id = "test.inventory.service";
    service.service_version = (UmiVersion){1U, 0U, 0U};
    service.provider_module_id = "test.inventory";
    service.service = &service_value;
    service.flags = UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE;
    assert(umi_service_registry_register(services, &service) == UMI_STATUS_OK);

    (void)memset(&capability, 0, sizeof(capability));
    capability.structure_size = (uint32_t)sizeof(capability);
    capability.capability_id = "test.inventory.capability";
    capability.capability_version = (UmiVersion){1U, 0U, 0U};
    capability.provider_module_id = "test.inventory";
    capability.provider = &service_value;
    capability.flags = UMI_CAPABILITY_SINGLETON | UMI_CAPABILITY_RELOAD_SAFE;
    assert(umi_capability_registry_register(capabilities, &capability) == UMI_STATUS_OK);

    assert(umi_runtime_inventory_snapshot(
        commands, services, capabilities, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.structure_size == sizeof(snapshot));
    assert(snapshot.api_version == UMI_RUNTIME_INVENTORY_API_VERSION);
    assert(snapshot.command_count == 1U);
    assert(snapshot.mutating_command_count == 1U);
    assert(snapshot.audited_command_count == 1U);
    assert(snapshot.background_command_count == 0U);
    assert(snapshot.service_count == 1U);
    assert(snapshot.singleton_service_count == 1U);
    assert(snapshot.thread_safe_service_count == 1U);
    assert(snapshot.capability_count == 1U);
    assert(snapshot.singleton_capability_count == 1U);
    assert(snapshot.reload_safe_capability_count == 1U);

    umi_capability_registry_destroy(capabilities);
    umi_service_registry_destroy(services);
    umi_command_registry_destroy(commands);
    return 0;
}
