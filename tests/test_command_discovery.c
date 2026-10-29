/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_command_discovery.c
 *
 * PURPOSE:
 *   Verify coherent command policy summaries and allocation-free prefix queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/runtime/inventory.h"
#include <stdio.h>
#include <string.h>

typedef struct Fixture {
    UmiCommandRegistry *registry;
    unsigned handler_calls;
    unsigned enabled_calls;
} Fixture;
static UmiStatus Handler(void *data, const char *argument, char *message, size_t capacity)
{
    Fixture *fixture = data;
    (void)argument;
    if (message != NULL && capacity != 0U) message[0] = '\0';
    ++fixture->handler_calls;
    return UMI_STATUS_OK;
}
static int Enabled(void *data, const char *argument)
{
    Fixture *fixture = data;
    (void)argument;
    ++fixture->enabled_calls;
    return 0;
}
static void Add(Fixture *fixture, const char *id, uint32_t flags)
{
    UmiCommandDescriptor descriptor = {0};
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = id;
    descriptor.title = "Discovery fixture";
    descriptor.category = "Verification";
    descriptor.description = "Metadata capture must not execute this command";
    descriptor.required_permission = "";
    descriptor.flags = flags;
    descriptor.handler = Handler;
    descriptor.enabled = Enabled;
    descriptor.user_data = fixture;
    assert(umi_command_registry_register(fixture->registry, &descriptor) == UMI_STATUS_OK);
}
static void CheckEmpty(void)
{
    Fixture fixture = {0};
    assert(umi_command_registry_create(&fixture.registry) == UMI_STATUS_OK);
    UmiCommandRegistrySummary summary = {99U, 99U, 99U, 99U, 99U};
    assert(UmiCommandRegistryCopySummary(fixture.registry, &summary) == UMI_STATUS_OK);
    assert(summary.command_count == 0U && summary.mutating_command_count == 0U &&
           summary.background_command_count == 0U && summary.audited_command_count == 0U &&
           summary.trusted_command_count == 0U);
    umi_command_registry_destroy(fixture.registry);
}
static void CheckInvalid(void)
{
    Fixture fixture = {0};
    assert(umi_command_registry_create(&fixture.registry) == UMI_STATUS_OK);
    UmiCommandRegistrySummary summary = {99U, 98U, 97U, 96U, 95U};
    assert(UmiCommandRegistryCopySummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    assert(summary.command_count == 99U && summary.mutating_command_count == 98U &&
           summary.background_command_count == 97U && summary.audited_command_count == 96U &&
           summary.trusted_command_count == 95U);
    assert(UmiCommandRegistryCopySummary(fixture.registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    size_t count = 0U;
    assert(umi_runtime_inventory_count_command_prefix(NULL, "", &count) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, NULL, &count) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "", NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    umi_command_registry_destroy(fixture.registry);
}
static void CheckFlags(const char *scenario)
{
    Fixture fixture = {0};
    assert(umi_command_registry_create(&fixture.registry) == UMI_STATUS_OK);
    for (uint32_t flags = 0U; flags < 16U; ++flags) {
        char id[48];
        int written = snprintf(id, sizeof(id), "matrix.command.%02u", (unsigned)flags);
        assert(written > 0 && (size_t)written < sizeof(id));
        Add(&fixture, id, flags);
    }
    UmiCommandRegistrySummary summary;
    assert(UmiCommandRegistryCopySummary(fixture.registry, &summary) == UMI_STATUS_OK);
    assert(summary.command_count == 16U && summary.mutating_command_count == 8U &&
           summary.background_command_count == 8U && summary.audited_command_count == 8U &&
           summary.trusted_command_count == 8U);
    UmiRuntimeInventorySnapshot inventory;
    assert(umi_runtime_inventory_snapshot(fixture.registry, NULL, NULL, &inventory) == UMI_STATUS_OK);
    assert(inventory.command_count == summary.command_count &&
           inventory.mutating_command_count == summary.mutating_command_count &&
           inventory.background_command_count == summary.background_command_count &&
           inventory.audited_command_count == summary.audited_command_count &&
           inventory.trusted_command_count == summary.trusted_command_count);
    assert(inventory.service_count == 0U && inventory.capability_count == 0U);
    if (strcmp(scenario, "mutation") == 0) {
        assert(umi_command_registry_unregister(fixture.registry, "matrix.command.15") == UMI_STATUS_OK);
        Add(&fixture, "matrix.command.15", UMI_COMMAND_NONE);
        assert(UmiCommandRegistryCopySummary(fixture.registry, &summary) == UMI_STATUS_OK);
        assert(summary.command_count == 16U && summary.mutating_command_count == 7U &&
               summary.background_command_count == 7U && summary.audited_command_count == 7U &&
               summary.trusted_command_count == 7U);
        /* The earlier report records the earlier observation and stays unchanged. */
        assert(inventory.mutating_command_count == 8U);
    }
    assert(fixture.handler_calls == 0U && fixture.enabled_calls == 0U);
    assert(!umi_command_registry_is_enabled(fixture.registry, "matrix.command.00", ""));
    assert(fixture.enabled_calls == 1U && fixture.handler_calls == 0U);
    umi_command_registry_destroy(fixture.registry);
    assert(summary.command_count == 16U);
}
static void CheckPrefix(void)
{
    Fixture fixture = {0};
    assert(umi_command_registry_create(&fixture.registry) == UMI_STATUS_OK);
    Add(&fixture, "document.open", UMI_COMMAND_NONE);
    Add(&fixture, "document.save", UMI_COMMAND_MUTATES_STATE);
    Add(&fixture, "workspace.build", UMI_COMMAND_BACKGROUND);
    size_t count = 99U;
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "", &count) ==
           UMI_STATUS_OK && count == 3U);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "document.", &count) ==
           UMI_STATUS_OK && count == 2U);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "document.open.extra", &count) ==
           UMI_STATUS_OK && count == 0U);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "missing", &count) ==
           UMI_STATUS_OK && count == 0U);
    assert(umi_command_registry_unregister(fixture.registry, "document.open") == UMI_STATUS_OK);
    assert(umi_runtime_inventory_count_command_prefix(fixture.registry, "document.", &count) ==
           UMI_STATUS_OK && count == 1U);
    assert(fixture.handler_calls == 0U && fixture.enabled_calls == 0U);
    umi_command_registry_destroy(fixture.registry);
}
int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "empty") == 0) CheckEmpty();
    else if (strcmp(argv[1], "invalid") == 0) CheckInvalid();
    else if (strcmp(argv[1], "flag-matrix") == 0 || strcmp(argv[1], "mutation") == 0)
        CheckFlags(argv[1]);
    else if (strcmp(argv[1], "prefix") == 0) CheckPrefix();
    else assert(!"Unknown command discovery case");
    return 0;
}
