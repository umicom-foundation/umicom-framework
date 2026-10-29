/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_service_snapshots.c
 *
 * PURPOSE:
 *   Verify copied service discovery survives mutation and owner release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/service_registry.h"
#include "umicom/runtime/inventory.h"
#include <stdio.h>
/* Preserve executable setup/checks in every build configuration. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <string.h>

static int service_objects[4];
static UmiServiceDescriptor Descriptor(const char *id, const char *provider, size_t object)
{
    UmiServiceDescriptor descriptor = {0};
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = id;
    descriptor.provider_module_id = provider;
    descriptor.service_version = (UmiVersion){1U, 2U, 3U};
    descriptor.service = &service_objects[object];
    return descriptor;
}
static UmiServiceRegistry *Create(void)
{
    UmiServiceRegistry *registry = NULL;
    assert(umi_service_registry_create(&registry) == UMI_STATUS_OK);
    return registry;
}
static void Add(UmiServiceRegistry *registry, const char *id, const char *provider, size_t object)
{
    UmiServiceDescriptor descriptor = Descriptor(id, provider, object);
    assert(umi_service_registry_register(registry, &descriptor) == UMI_STATUS_OK);
}
static void CheckEmpty(void)
{
    UmiServiceRegistry *registry = Create();
    size_t total = 99U;
    assert(UmiServiceRegistryCopySnapshots(registry, NULL, 0U, &total) == UMI_STATUS_OK);
    assert(total == 0U);
    UmiServiceSnapshot output;
    (void)memset(&output, 0x5a, sizeof(output));
    UmiServiceSnapshot before;
    (void)memcpy(&before, &output, sizeof(before));
    assert(UmiServiceRegistryCopySnapshots(registry, &output, 1U, &total) == UMI_STATUS_OK);
    assert(total == 0U && memcmp(&output, &before, sizeof(output)) == 0);
    assert(UmiServiceRegistryCopySnapshot(registry, "missing", NULL, &output) == UMI_STATUS_NOT_FOUND);
    assert(memcmp(&output, &before, sizeof(output)) == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckLookup(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    UmiServiceSnapshot output;
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, &output) == UMI_STATUS_OK);
    assert(strcmp(output.service_id, "example.one") == 0);
    assert(strcmp(output.provider_module_id, "provider.one") == 0);
    assert(output.service_version.major == 1U && output.service_version.minor == 2U &&
           output.service_version.patch == 3U && output.flags == UMI_SERVICE_NONE);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", "provider.one", &output) ==
           UMI_STATUS_OK);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckMissingProvider(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    UmiServiceSnapshot output;
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, &output) == UMI_STATUS_OK);
    UmiServiceSnapshot before;
    (void)memcpy(&before, &output, sizeof(before));
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", "other", &output) ==
           UMI_STATUS_NOT_FOUND);
    assert(memcmp(&output, &before, sizeof(output)) == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckMultipleProviders(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.shared", "provider.one", 0U);
    Add(registry, "example.shared", "provider.two", 1U);
    UmiServiceSnapshot output[2];
    size_t total = 0U;
    assert(UmiServiceRegistryCopySnapshots(registry, output, 2U, &total) == UMI_STATUS_OK);
    assert(total == 2U);
    assert(strcmp(output[0].provider_module_id, "provider.one") == 0);
    assert(strcmp(output[1].provider_module_id, "provider.two") == 0);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.shared", NULL, output) == UMI_STATUS_OK);
    assert(strcmp(output[0].provider_module_id, "provider.one") == 0);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.shared", "provider.two", output) ==
           UMI_STATUS_OK);
    assert(strcmp(output[0].provider_module_id, "provider.two") == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckTruncation(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    Add(registry, "example.two", "provider.two", 1U);
    UmiServiceSnapshot output[2];
    (void)memset(output, 0x5a, sizeof(output));
    UmiServiceSnapshot untouched;
    (void)memcpy(&untouched, &output[1], sizeof(untouched));
    size_t total = 0U;
    assert(UmiServiceRegistryCopySnapshots(registry, output, 1U, &total) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(total == 2U && strcmp(output[0].service_id, "example.one") == 0);
    assert(memcmp(&output[1], &untouched, sizeof(untouched)) == 0);
    UmiServiceSnapshot before;
    (void)memcpy(&before, &output[0], sizeof(before));
    assert(UmiServiceRegistryCopySnapshots(registry, output, 0U, &total) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(total == 2U && memcmp(output, &before, sizeof(before)) == 0);
    assert(UmiServiceRegistryCopySnapshots(registry, NULL, 0U, &total) == UMI_STATUS_OK);
    assert(total == 2U);
    assert(UmiServiceRegistryCopySnapshots(registry, output, 2U, &total) == UMI_STATUS_OK);
    assert(strcmp(output[1].service_id, "example.two") == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckInvalidArguments(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    UmiServiceSnapshot output;
    (void)memset(&output, 0x5a, sizeof(output));
    UmiServiceSnapshot before;
    (void)memcpy(&before, &output, sizeof(before));
    size_t total = 99U;
    assert(UmiServiceRegistryCopySnapshots(NULL, &output, 1U, &total) == UMI_STATUS_INVALID_ARGUMENT);
    assert(total == 0U);
    total = 99U;
    assert(UmiServiceRegistryCopySnapshots(registry, NULL, 1U, &total) == UMI_STATUS_INVALID_ARGUMENT);
    assert(total == 0U);
    assert(UmiServiceRegistryCopySnapshots(registry, &output, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySnapshot(NULL, "example.one", NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySnapshot(registry, NULL, NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySnapshot(registry, "", NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", "", &output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&output, &before, sizeof(output)) == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckCompaction(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    Add(registry, "example.two", "provider.two", 1U);
    Add(registry, "example.three", "provider.three", 2U);
    UmiServiceSnapshot saved[3], current[3];
    size_t total = 0U;
    assert(UmiServiceRegistryCopySnapshots(registry, saved, 3U, &total) == UMI_STATUS_OK);
    assert(total == 3U);
    assert(umi_service_registry_unregister(registry, "example.one", "provider.one") == UMI_STATUS_OK);
    Add(registry, "example.four", "provider.four", 3U);
    assert(UmiServiceRegistryCopySnapshots(registry, current, 3U, &total) == UMI_STATUS_OK);
    assert(total == 3U);
    assert(strcmp(current[0].service_id, "example.two") == 0);
    assert(strcmp(current[1].service_id, "example.three") == 0);
    assert(strcmp(current[2].service_id, "example.four") == 0);
    assert(strcmp(saved[0].service_id, "example.one") == 0);
    assert(strcmp(saved[1].provider_module_id, "provider.two") == 0);
    assert(strcmp(saved[2].provider_module_id, "provider.three") == 0);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckReplacement(void)
{
    UmiServiceRegistry *registry = Create();
    UmiServiceDescriptor descriptor = Descriptor("example.one", "provider.one", 0U);
    descriptor.flags = UMI_SERVICE_REPLACEABLE;
    assert(umi_service_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    UmiServiceSnapshot saved, current;
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, &saved) == UMI_STATUS_OK);
    descriptor.service_version = (UmiVersion){2U, 0U, 0U};
    descriptor.flags = UMI_SERVICE_SINGLETON;
    descriptor.service = &service_objects[1];
    assert(umi_service_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, &current) == UMI_STATUS_OK);
    assert(saved.service_version.major == 1U && saved.flags == UMI_SERVICE_REPLACEABLE);
    assert(current.service_version.major == 2U && current.flags == UMI_SERVICE_SINGLETON);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}
static void CheckAfterRelease(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.one", "provider.one", 0U);
    UmiServiceSnapshot saved;
    assert(UmiServiceRegistryCopySnapshot(registry, "example.one", NULL, &saved) == UMI_STATUS_OK);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
    assert(registry == NULL);
    assert(strcmp(saved.service_id, "example.one") == 0);
    assert(strcmp(saved.provider_module_id, "provider.one") == 0);
    assert(saved.service_version.patch == 3U);
}
typedef struct CallbackState {
    UmiServiceRegistry *registry;
    unsigned called;
    UmiServiceSnapshot remaining;
} CallbackState;
static void InspectRetirement(void *object, void *user_data)
{
    CallbackState *state = user_data;
    assert(object == &service_objects[1]);
    assert(UmiServiceRegistryIsInCallback(state->registry));
    size_t total = 99U;
    assert(UmiServiceRegistryCopySnapshots(state->registry, &state->remaining, 1U, &total) ==
           UMI_STATUS_OK);
    assert(total == 1U && strcmp(state->remaining.service_id, "example.kept") == 0);
    UmiServiceSnapshot untouched;
    (void)memcpy(&untouched, &state->remaining, sizeof(untouched));
    assert(UmiServiceRegistryCopySnapshot(state->registry, "example.retired", NULL,
        &state->remaining) == UMI_STATUS_NOT_FOUND);
    assert(memcmp(&untouched, &state->remaining, sizeof(untouched)) == 0);
    UmiServiceRegistrySummary summary;
    assert(UmiServiceRegistryCopySummary(state->registry, &summary) == UMI_STATUS_OK);
    assert(summary.service_count == 1U && summary.owned_service_count == 0U);
    assert(UmiServiceRegistryCountPrefix(state->registry, "example.", &total) == UMI_STATUS_OK);
    assert(total == 1U);
    ++state->called;
}
static void CheckDestructorMetadata(void)
{
    CallbackState state = {0};
    state.registry = Create();
    Add(state.registry, "example.kept", "provider.one", 0U);
    UmiServiceDescriptor owned = Descriptor("example.retired", "provider.two", 1U);
    owned.flags = UMI_SERVICE_OWNED;
    owned.destroy = InspectRetirement;
    owned.destroy_user_data = &state;
    assert(umi_service_registry_register(state.registry, &owned) == UMI_STATUS_OK);
    assert(umi_service_registry_unregister(state.registry, "example.retired", "provider.two") ==
           UMI_STATUS_OK);
    assert(state.called == 1U);
    assert(UmiServiceRegistryRelease(&state.registry) == UMI_STATUS_OK);
    assert(strcmp(state.remaining.provider_module_id, "provider.one") == 0);
}

static void CheckSummaryFlags(void)
{
    UmiServiceRegistry *registry = Create();
    UmiServiceRegistrySummary summary = {99U, 99U, 99U, 99U, 99U};
    assert(UmiServiceRegistryCopySummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    assert(summary.service_count == 99U && summary.replaceable_service_count == 99U);
    assert(UmiServiceRegistryCopySummary(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiServiceRegistryCopySummary(registry, &summary) == UMI_STATUS_OK);
    assert(summary.service_count == 0U && summary.owned_service_count == 0U);
    for (uint32_t flags = 0U; flags < 16U; ++flags) {
        char id[48];
        int written = snprintf(id, sizeof(id), "matrix.service.%02u", (unsigned)flags);
        assert(written > 0 && (size_t)written < sizeof(id));
        UmiServiceDescriptor descriptor = Descriptor(id, "matrix.provider", 0U);
        /* Metadata classification does not install a destructor for this static
         * object. OWNED has no destroy hook in this deliberate flag fixture. */
        descriptor.flags = flags;
        assert(umi_service_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    }
    assert(UmiServiceRegistryCopySummary(registry, &summary) == UMI_STATUS_OK);
    assert(summary.service_count == 16U && summary.singleton_service_count == 8U &&
           summary.owned_service_count == 8U && summary.thread_safe_service_count == 8U &&
           summary.replaceable_service_count == 8U);
    UmiRuntimeInventorySnapshot inventory;
    assert(umi_runtime_inventory_snapshot(NULL, registry, NULL, &inventory) == UMI_STATUS_OK);
    assert(inventory.service_count == summary.service_count &&
           inventory.owned_service_count == summary.owned_service_count &&
           inventory.singleton_service_count == summary.singleton_service_count &&
           inventory.thread_safe_service_count == summary.thread_safe_service_count &&
           inventory.replaceable_service_count == summary.replaceable_service_count);
    assert(inventory.command_count == 0U && inventory.capability_count == 0U);
    assert(umi_service_registry_unregister(registry, "matrix.service.00", "matrix.provider") ==
           UMI_STATUS_OK);
    UmiServiceDescriptor replacement = Descriptor("matrix.service.15", "matrix.provider", 0U);
    assert(umi_service_registry_register(registry, &replacement) == UMI_STATUS_OK);
    assert(UmiServiceRegistryCopySummary(registry, &summary) == UMI_STATUS_OK);
    assert(summary.service_count == 15U && summary.singleton_service_count == 7U &&
           summary.owned_service_count == 7U && summary.thread_safe_service_count == 7U &&
           summary.replaceable_service_count == 7U);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
    assert(summary.service_count == 15U);
}
static void CheckPrefix(void)
{
    UmiServiceRegistry *registry = Create();
    Add(registry, "example.alpha", "provider.one", 0U);
    Add(registry, "example.alpha", "provider.two", 1U);
    Add(registry, "other.beta", "provider.three", 2U);
    size_t count = 99U;
    assert(UmiServiceRegistryCountPrefix(registry, "", &count) == UMI_STATUS_OK && count == 3U);
    assert(UmiServiceRegistryCountPrefix(registry, "example.", &count) == UMI_STATUS_OK && count == 2U);
    assert(UmiServiceRegistryCountPrefix(registry, "example.alpha", &count) == UMI_STATUS_OK && count == 2U);
    assert(UmiServiceRegistryCountPrefix(registry, "example.alpha.longer", &count) == UMI_STATUS_OK &&
           count == 0U);
    assert(UmiServiceRegistryCountPrefix(registry, "missing", &count) == UMI_STATUS_OK && count == 0U);
    count = 99U;
    assert(UmiServiceRegistryCountPrefix(NULL, "", &count) == UMI_STATUS_INVALID_ARGUMENT && count == 0U);
    count = 99U;
    assert(UmiServiceRegistryCountPrefix(registry, NULL, &count) == UMI_STATUS_INVALID_ARGUMENT &&
           count == 0U);
    assert(UmiServiceRegistryCountPrefix(registry, "", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_inventory_has_service(registry, "example.alpha"));
    assert(umi_service_registry_unregister(registry, "example.alpha", "provider.one") == UMI_STATUS_OK);
    assert(umi_runtime_inventory_count_service_prefix(registry, "example.", &count) == UMI_STATUS_OK &&
           count == 1U);
    assert(umi_service_registry_unregister(registry, "example.alpha", "provider.two") == UMI_STATUS_OK);
    assert(!umi_runtime_inventory_has_service(registry, "example.alpha"));
    assert(!umi_runtime_inventory_has_service(NULL, "example.alpha"));
    assert(!umi_runtime_inventory_has_service(registry, NULL));
    assert(umi_runtime_inventory_count_service_prefix(registry, "", &count) == UMI_STATUS_OK &&
           count == 1U);
    assert(UmiServiceRegistryRelease(&registry) == UMI_STATUS_OK);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "empty") == 0) CheckEmpty();
    else if (strcmp(argv[1], "lookup") == 0) CheckLookup();
    else if (strcmp(argv[1], "missing-provider") == 0) CheckMissingProvider();
    else if (strcmp(argv[1], "multiple-providers") == 0) CheckMultipleProviders();
    else if (strcmp(argv[1], "truncation") == 0) CheckTruncation();
    else if (strcmp(argv[1], "invalid-arguments") == 0) CheckInvalidArguments();
    else if (strcmp(argv[1], "compaction") == 0) CheckCompaction();
    else if (strcmp(argv[1], "replacement") == 0) CheckReplacement();
    else if (strcmp(argv[1], "after-release") == 0) CheckAfterRelease();
    else if (strcmp(argv[1], "destructor-metadata") == 0) CheckDestructorMetadata();
    else if (strcmp(argv[1], "summary-flags") == 0) CheckSummaryFlags();
    else if (strcmp(argv[1], "prefix") == 0) CheckPrefix();
    else assert(!"Unknown metadata snapshot case");
    return 0;
}
