/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_inventory_queries.c
 *
 * PURPOSE:
 *   Verify reusable runtime namespace and existence queries across command,
 *   service and capability registries.
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
    UmiCommandDescriptor command;
    UmiServiceDescriptor service;
    UmiCapabilityDescriptor capability;
    size_t count = 0U;
    int provider = 1;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_service_registry_create(&services) == UMI_STATUS_OK);
    assert(umi_capability_registry_create(&capabilities) == UMI_STATUS_OK);

    (void)memset(&command, 0, sizeof(command));
    command.structure_size = (uint32_t)sizeof(command);
    command.command_id = "org.umicom.alpha.command";
    command.title = "Alpha";
    command.category = "Tests";
    command.description = "Alpha command";
    command.required_permission = "";
    command.handler = noop_command;
    assert(umi_command_registry_register(commands, &command) == UMI_STATUS_OK);
    command.command_id = "org.umicom.beta.command";
    command.title = "Beta";
    assert(umi_command_registry_register(commands, &command) == UMI_STATUS_OK);

    (void)memset(&service, 0, sizeof(service));
    service.structure_size = (uint32_t)sizeof(service);
    service.service_id = "org.umicom.alpha.service";
    service.service_version = (UmiVersion){1U, 0U, 0U};
    service.provider_module_id = "tests";
    service.service = &provider;
    assert(umi_service_registry_register(services, &service) == UMI_STATUS_OK);

    (void)memset(&capability, 0, sizeof(capability));
    capability.structure_size = (uint32_t)sizeof(capability);
    capability.capability_id = "org.umicom.alpha.capability";
    capability.capability_version = (UmiVersion){1U, 0U, 0U};
    capability.provider_module_id = "tests";
    capability.provider = &provider;
    assert(umi_capability_registry_register(capabilities, &capability) == UMI_STATUS_OK);

    assert(umi_runtime_inventory_has_command(commands,
                                             "org.umicom.alpha.command"));
    assert(!umi_runtime_inventory_has_command(commands, "missing"));
    assert(umi_runtime_inventory_has_service(services,
                                             "org.umicom.alpha.service"));
    assert(umi_runtime_inventory_has_capability(
        capabilities, "org.umicom.alpha.capability"));

    assert(umi_runtime_inventory_count_command_prefix(
        commands, "org.umicom.alpha", &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(umi_runtime_inventory_count_command_prefix(
        commands, "org.umicom.", &count) == UMI_STATUS_OK);
    assert(count == 2U);
    assert(umi_runtime_inventory_count_service_prefix(
        services, "org.umicom.alpha", &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(umi_runtime_inventory_count_capability_prefix(
        capabilities, "org.umicom.alpha", &count) == UMI_STATUS_OK);
    assert(count == 1U);

    umi_capability_registry_destroy(capabilities);
    umi_service_registry_destroy(services);
    umi_command_registry_destroy(commands);
    return 0;
}
