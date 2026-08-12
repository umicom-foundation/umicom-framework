/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_command_registry.c
 *
 * PURPOSE:
 *   Verify command metadata, enablement, execution, duplicate protection,
 *   snapshots and unregistration through the shared command registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct CommandFixture {
    int enabled;
    int execution_count;
} CommandFixture;

static int command_enabled(void *user_data, const char *argument)
{
    CommandFixture *fixture = (CommandFixture *)user_data;
    (void)argument;
    return fixture->enabled;
}

static UmiStatus command_handler(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t message_capacity)
{
    CommandFixture *fixture = (CommandFixture *)user_data;
    fixture->execution_count += 1;
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "executed:%s",
                       argument);
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiCommandRegistry *registry = NULL;
    UmiCommandDescriptor descriptor;
    UmiCommandSnapshot snapshot;
    CommandFixture fixture = {1, 0};
    char message[128];

    assert(umi_command_registry_create(&registry) == UMI_STATUS_OK);

    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "studio.test.command";
    descriptor.title = "Test command";
    descriptor.category = "Testing";
    descriptor.description = "Exercises the command registry.";
    descriptor.required_permission = "studio.test.execute";
    descriptor.flags = UMI_COMMAND_AUDITED | UMI_COMMAND_MUTATES_STATE;
    descriptor.handler = command_handler;
    descriptor.enabled = command_enabled;
    descriptor.user_data = &fixture;

    assert(umi_command_registry_register(registry, &descriptor) ==
           UMI_STATUS_OK);
    assert(umi_command_registry_register(registry, &descriptor) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_command_registry_count(registry) == 1U);
    assert(umi_command_registry_snapshot(registry,
                                         descriptor.command_id,
                                         &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.title, descriptor.title) == 0);
    assert(umi_command_registry_is_enabled(registry,
                                           descriptor.command_id,
                                           "value"));

    assert(umi_command_registry_execute(registry,
                                        descriptor.command_id,
                                        "value",
                                        message,
                                        sizeof(message)) == UMI_STATUS_OK);
    assert(strcmp(message, "executed:value") == 0);
    assert(fixture.execution_count == 1);

    fixture.enabled = 0;
    assert(!umi_command_registry_is_enabled(registry,
                                            descriptor.command_id,
                                            ""));
    assert(umi_command_registry_execute(registry,
                                        descriptor.command_id,
                                        "",
                                        message,
                                        sizeof(message)) ==
           UMI_STATUS_INVALID_STATE);

    assert(umi_command_registry_unregister(registry,
                                           descriptor.command_id) ==
           UMI_STATUS_OK);
    assert(umi_command_registry_count(registry) == 0U);

    {
        UmiCommandBatchReport report;
        UmiCommandDescriptor batch[2];
        UmiCommandDescriptor invalid_batch[2];
        UmiCommandSnapshot matches[2];
        size_t match_count = 0U;

        fixture.enabled = 1;
        descriptor.command_id = "studio.compat.permissionless";
        descriptor.required_permission = NULL;
        assert(umi_command_registry_register(registry, &descriptor) ==
               UMI_STATUS_OK);
        assert(umi_command_registry_contains(
            registry, "studio.compat.permissionless"));
        assert(umi_command_registry_snapshot(
            registry, "studio.compat.permissionless", &snapshot) ==
               UMI_STATUS_OK);
        assert(strcmp(snapshot.required_permission, "") == 0);
        assert(umi_command_registry_unregister(
            registry, "studio.compat.permissionless") == UMI_STATUS_OK);

        batch[0] = descriptor;
        batch[0].command_id = "studio.batch.one";
        batch[0].title = "Batch One";
        batch[0].required_permission = NULL;
        batch[1] = descriptor;
        batch[1].command_id = "studio.batch.two";
        batch[1].title = "Batch Two";
        batch[1].required_permission = "studio.batch.execute";

        assert(umi_command_registry_register_many(
            registry, batch, 2U, &report) == UMI_STATUS_OK);
        assert(report.api_version == UMI_COMMAND_BATCH_API_VERSION);
        assert(report.requested_count == 2U);
        assert(report.registered_count == 2U);
        assert(report.failed_index == SIZE_MAX);
        assert(umi_command_registry_count(registry) == 2U);

        assert(umi_command_registry_find_prefix(
            registry, "studio.batch.", matches, 2U, &match_count) ==
               UMI_STATUS_OK);
        assert(match_count == 2U);
        assert(strcmp(matches[0].command_id, "studio.batch.one") == 0);
        assert(strcmp(matches[1].command_id, "studio.batch.two") == 0);

        invalid_batch[0] = descriptor;
        invalid_batch[0].command_id = "studio.atomic.new";
        invalid_batch[0].required_permission = "";
        invalid_batch[1] = descriptor;
        invalid_batch[1].command_id = "studio.batch.one";
        invalid_batch[1].required_permission = "";

        assert(umi_command_registry_register_many(
            registry, invalid_batch, 2U, &report) ==
               UMI_STATUS_ALREADY_EXISTS);
        assert(report.registered_count == 0U);
        assert(report.failed_index == 1U);
        assert(!umi_command_registry_contains(registry, "studio.atomic.new"));
        assert(umi_command_registry_count(registry) == 2U);

        assert(umi_command_registry_unregister(
            registry, "studio.batch.one") == UMI_STATUS_OK);
        assert(umi_command_registry_unregister(
            registry, "studio.batch.two") == UMI_STATUS_OK);
        assert(umi_command_registry_count(registry) == 0U);
    }

    umi_command_registry_destroy(registry);
    return 0;
}
