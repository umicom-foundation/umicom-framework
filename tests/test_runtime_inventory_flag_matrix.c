/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_inventory_flag_matrix.c
 *
 * PURPOSE:
 *   Verify every command, service and capability flag classification exported
 *   by the runtime inventory snapshot.
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

/*
 * Exercise handler and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus handler(void *user_data,
                         const char *argument,
                         char *out_message,
                         size_t message_capacity)
{
    (void)user_data;
    (void)argument;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) out_message[0] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiServiceRegistry *services = NULL;
    UmiCapabilityRegistry *capabilities = NULL;
    UmiCommandDescriptor command;
    UmiServiceDescriptor service;
    UmiCapabilityDescriptor capability;
    UmiRuntimeInventorySnapshot snapshot;
    int value = 1;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_service_registry_create(&services) == UMI_STATUS_OK);
    assert(umi_capability_registry_create(&capabilities) == UMI_STATUS_OK);

    (void)memset(&command, 0, sizeof(command));
    command.structure_size = (uint32_t)sizeof(command);
    command.command_id = "flags.command";
    command.title = "Flags";
    command.category = "Tests";
    command.description = "All command flags";
    command.required_permission = "";
    command.flags = UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_BACKGROUND |
                    UMI_COMMAND_AUDITED | UMI_COMMAND_REQUIRES_TRUST;
    command.handler = handler;
    assert(umi_command_registry_register(commands, &command) == UMI_STATUS_OK);

    (void)memset(&service, 0, sizeof(service));
    service.structure_size = (uint32_t)sizeof(service);
    service.service_id = "flags.service";
    service.service_version = (UmiVersion){1U, 0U, 0U};
    service.provider_module_id = "flags";
    service.service = &value;
    service.flags = UMI_SERVICE_SINGLETON | UMI_SERVICE_OWNED |
                    UMI_SERVICE_THREAD_SAFE | UMI_SERVICE_REPLACEABLE;
    assert(umi_service_registry_register(services, &service) == UMI_STATUS_OK);

    (void)memset(&capability, 0, sizeof(capability));
    capability.structure_size = (uint32_t)sizeof(capability);
    capability.capability_id = "flags.capability";
    capability.capability_version = (UmiVersion){1U, 0U, 0U};
    capability.provider_module_id = "flags";
    capability.provider = &value;
    capability.flags = UMI_CAPABILITY_SINGLETON | UMI_CAPABILITY_EXTERNAL |
                       UMI_CAPABILITY_RELOAD_SAFE | UMI_CAPABILITY_OPTIONAL;
    assert(umi_capability_registry_register(capabilities, &capability) ==
           UMI_STATUS_OK);

    assert(umi_runtime_inventory_snapshot(
        commands, services, capabilities, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.mutating_command_count == 1U);
    assert(snapshot.background_command_count == 1U);
    assert(snapshot.audited_command_count == 1U);
    assert(snapshot.trusted_command_count == 1U);
    assert(snapshot.singleton_service_count == 1U);
    assert(snapshot.owned_service_count == 1U);
    assert(snapshot.thread_safe_service_count == 1U);
    assert(snapshot.replaceable_service_count == 1U);
    assert(snapshot.singleton_capability_count == 1U);
    assert(snapshot.external_capability_count == 1U);
    assert(snapshot.reload_safe_capability_count == 1U);
    assert(snapshot.optional_capability_count == 1U);

    /* UMI_SERVICE_OWNED is safe here because the descriptor has no destroy hook. */
    umi_capability_registry_destroy(capabilities);
    umi_service_registry_destroy(services);
    umi_command_registry_destroy(commands);
    return 0;
}
