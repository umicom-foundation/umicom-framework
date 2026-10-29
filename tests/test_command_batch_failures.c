/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_command_batch_failures.c
 *
 * PURPOSE:
 *   Verify exact rejected command indexes and unchanged batch membership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/runtime/command_registry.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static UmiStatus Run(void *data, const char *argument, char *message, size_t capacity)
{
    (void)data;
    (void)argument;
    if (message != NULL && capacity != 0U) message[0] = '\0';
    return UMI_STATUS_OK;
}
static UmiCommandDescriptor Describe(const char *id)
{
    UmiCommandDescriptor value = {0};
    value.structure_size = (uint32_t)sizeof(value);
    value.command_id = id;
    value.title = id;
    value.category = "Example";
    value.description = "Command admission fixture";
    value.handler = Run;
    return value;
}
static void CheckReport(const UmiCommandBatchReport *report, UmiStatus status,
                        size_t count, size_t rejected)
{
    assert(report->structure_size == (uint32_t)sizeof(*report));
    assert(report->api_version == UMI_COMMAND_BATCH_API_VERSION);
    assert(report->status == status);
    assert(report->requested_count == count);
    assert(report->registered_count == 0U);
    assert(report->failed_index == rejected);
}
static void CheckEntry(size_t failed, int existing, int malformed, int report_enabled)
{
    UmiCommandRegistry *registry = NULL;
    assert(umi_command_registry_create(&registry) == UMI_STATUS_OK);
    const char *ids[] = {"example.first", "example.second", "example.third", "example.fourth"};
    UmiCommandDescriptor batch[4];
    for (size_t index = 0U; index < 4U; ++index) batch[index] = Describe(ids[index]);
    if (existing) {
        UmiCommandDescriptor original = Describe("example.existing");
        assert(umi_command_registry_register(registry, &original) == UMI_STATUS_OK);
        batch[failed].command_id = original.command_id;
    } else if (malformed) batch[failed].handler = NULL;
    else batch[failed].command_id = batch[0].command_id;
    UmiCommandBatchReport report;
    memset(&report, 0xa5, sizeof(report));
    UmiStatus expected = malformed ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_ALREADY_EXISTS;
    assert(umi_command_registry_register_many(registry, batch, 4U,
        report_enabled ? &report : NULL) == expected);
    if (report_enabled) CheckReport(&report, expected, 4U, failed);
    assert(umi_command_registry_count(registry) == (existing ? 1U : 0U));
    for (size_t index = 0U; index < 4U; ++index)
        assert(!umi_command_registry_contains(registry, ids[index]));
    if (existing) assert(umi_command_registry_contains(registry, "example.existing"));
    batch[failed] = Describe(ids[failed]);
    assert(umi_command_registry_register_many(registry, batch, 4U, &report) == UMI_STATUS_OK);
    assert(report.status == UMI_STATUS_OK && report.registered_count == 4U);
    assert(report.failed_index == SIZE_MAX);
    assert(umi_command_registry_count(registry) == (existing ? 5U : 4U));
    umi_command_registry_destroy(registry);
}
static void CheckGlobal(void)
{
    UmiCommandRegistry *registry = NULL;
    UmiCommandBatchReport report;
    UmiCommandDescriptor pair[] = {Describe("example.one"), Describe("example.two")};
    assert(umi_command_registry_register_many(NULL, pair, 2U, &report)
           == UMI_STATUS_INVALID_ARGUMENT);
    CheckReport(&report, UMI_STATUS_INVALID_ARGUMENT, 2U, SIZE_MAX);
    assert(umi_command_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_command_registry_register_many(registry, NULL, 1U, &report)
           == UMI_STATUS_INVALID_ARGUMENT);
    CheckReport(&report, UMI_STATUS_INVALID_ARGUMENT, 1U, SIZE_MAX);
    assert(umi_command_registry_register_many(registry, pair,
        UMI_COMMAND_REGISTRY_MAX + 1U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    CheckReport(&report, UMI_STATUS_CAPACITY_EXCEEDED, UMI_COMMAND_REGISTRY_MAX + 1U, SIZE_MAX);
    for (size_t index = 0U; index < UMI_COMMAND_REGISTRY_MAX - 1U; ++index) {
        char id[64];
        int written = snprintf(id, sizeof(id), "example.capacity.%zu", index);
        assert(written > 0 && (size_t)written < sizeof(id));
        UmiCommandDescriptor value = Describe(id);
        assert(umi_command_registry_register(registry, &value) == UMI_STATUS_OK);
    }
    assert(umi_command_registry_register_many(registry, pair, 2U, &report)
           == UMI_STATUS_CAPACITY_EXCEEDED);
    CheckReport(&report, UMI_STATUS_CAPACITY_EXCEEDED, 2U, SIZE_MAX);
    assert(umi_command_registry_count(registry) == UMI_COMMAND_REGISTRY_MAX - 1U);
    assert(!umi_command_registry_contains(registry, "example.one"));
    assert(umi_command_registry_register_many(registry, pair, 1U, &report) == UMI_STATUS_OK);
    assert(report.failed_index == SIZE_MAX && report.registered_count == 1U);
    umi_command_registry_destroy(registry);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "duplicate-second") == 0) CheckEntry(1U, 0, 0, 1);
    else if (strcmp(argv[1], "duplicate-middle") == 0) CheckEntry(2U, 0, 0, 1);
    else if (strcmp(argv[1], "duplicate-last") == 0) CheckEntry(3U, 0, 0, 1);
    else if (strcmp(argv[1], "existing-first") == 0) CheckEntry(0U, 1, 0, 1);
    else if (strcmp(argv[1], "invalid-last") == 0) CheckEntry(3U, 0, 1, 1);
    else if (strcmp(argv[1], "without-report") == 0) CheckEntry(3U, 0, 0, 0);
    else if (strcmp(argv[1], "global-errors") == 0) CheckGlobal();
    else assert(!"Unknown command admission case");
    return 0;
}
