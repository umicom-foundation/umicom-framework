/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_registration_transactions.c
 *
 * PURPOSE:
 *   Verify atomic provider admission, identifier relocation and corrected retries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/runtime/master_controller.h"
#include "umicom/runtime/module_registry.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiCapabilityDescriptor Capability(const char *id, const char *provider,
                                          uint32_t flags)
{
    UmiCapabilityDescriptor value = {0};
    value.structure_size = (uint32_t)sizeof(value);
    value.capability_id = id;
    value.capability_version = (UmiVersion){1U, 0U, 0U};
    value.provider_module_id = provider;
    value.flags = flags;
    return value;
}

static void CheckBatch(const char *scenario)
{
    UmiCapabilityRegistry *registry = NULL;
    assert(umi_capability_registry_create(&registry) == UMI_STATUS_OK);
    UmiCapabilityDescriptor entries[3] = {
        Capability("example.alpha", "example.first", UMI_CAPABILITY_NONE),
        Capability("example.beta", "example.second", UMI_CAPABILITY_NONE),
        Capability("example.gamma", "example.third", UMI_CAPABILITY_NONE)
    };
    if (strcmp(scenario, "empty") == 0) {
        assert(UmiCapabilityRegistryRegisterBatch(NULL, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiCapabilityRegistryRegisterBatch(registry, NULL, 0U) == UMI_STATUS_OK);
        assert(UmiCapabilityRegistryRegisterBatch(registry, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries,
            UMI_CAPABILITY_REGISTRY_MAX + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(umi_capability_registry_count(registry) == 0U);
    } else if (strcmp(scenario, "batch") == 0) {
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) == UMI_STATUS_OK);
        for (size_t index = 0U; index < 3U; ++index) {
            const UmiCapabilityDescriptor *found = umi_capability_registry_at(registry, index);
            assert(found != NULL);
            assert(strcmp(found->capability_id, entries[index].capability_id) == 0);
            assert(strcmp(found->provider_module_id, entries[index].provider_module_id) == 0);
        }
    } else if (strcmp(scenario, "invalid-last") == 0) {
        char too_long[UMI_CAPABILITY_ID_CAPACITY + 1U];
        memset(too_long, 'x', sizeof(too_long) - 1U);
        too_long[sizeof(too_long) - 1U] = '\0';
        const char *bad[] = {NULL, "", too_long};
        for (size_t index = 0U; index < 3U; ++index) {
            entries[2].capability_id = bad[index];
            assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) ==
                   (index == 2U ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT));
            assert(umi_capability_registry_count(registry) == 0U);
            assert(umi_capability_registry_find(registry, "example.alpha") == NULL);
        }
        entries[2].capability_id = "example.gamma";
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) == UMI_STATUS_OK);
    } else if (strcmp(scenario, "duplicate-last") == 0) {
        assert(umi_capability_registry_register(registry, &entries[2]) == UMI_STATUS_OK);
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) == UMI_STATUS_ALREADY_EXISTS);
        assert(umi_capability_registry_count(registry) == 1U);
        assert(umi_capability_registry_find(registry, "example.alpha") == NULL);
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 2U) == UMI_STATUS_OK);
    } else if (strcmp(scenario, "batch-duplicate") == 0) {
        entries[2] = entries[0];
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) == UMI_STATUS_ALREADY_EXISTS);
        assert(umi_capability_registry_count(registry) == 0U);
    } else if (strcmp(scenario, "singleton-existing") == 0 ||
               strcmp(scenario, "singleton-incoming") == 0) {
        int existing = strcmp(scenario, "singleton-existing") == 0;
        entries[1].capability_id = entries[0].capability_id;
        entries[existing ? 0 : 1].flags = UMI_CAPABILITY_SINGLETON;
        assert(umi_capability_registry_register(registry, &entries[0]) == UMI_STATUS_OK);
        assert(umi_capability_registry_register(registry, &entries[1]) == UMI_STATUS_ALREADY_EXISTS);
        assert(umi_capability_registry_count(registry) == 1U);
    } else if (strcmp(scenario, "multiple-providers") == 0) {
        entries[1].capability_id = entries[0].capability_id;
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 2U) == UMI_STATUS_OK);
        assert(umi_capability_registry_find_provider(registry,
            entries[0].capability_id, entries[1].provider_module_id) != NULL);
    } else if (strcmp(scenario, "compaction") == 0) {
        assert(UmiCapabilityRegistryRegisterBatch(registry, entries, 3U) == UMI_STATUS_OK);
        assert(umi_capability_registry_unregister(registry, "example.alpha", "example.first")
               == UMI_STATUS_OK);
        /* Reuse the old tail, exposing stale self-pointers left by memmove. */
        UmiCapabilityDescriptor replacement = Capability("example.delta", "example.fourth", 0U);
        assert(umi_capability_registry_register(registry, &replacement) == UMI_STATUS_OK);
        for (size_t index = 1U; index < 3U; ++index) {
            const UmiCapabilityDescriptor *found = umi_capability_registry_find(
                registry, entries[index].capability_id);
            assert(found != NULL);
            assert(strcmp(found->capability_id, entries[index].capability_id) == 0);
            assert(strcmp(found->provider_module_id, entries[index].provider_module_id) == 0);
        }
        assert(umi_capability_registry_unregister(registry, "example.beta", "example.second")
               == UMI_STATUS_OK);
        const UmiCapabilityDescriptor *remaining = umi_capability_registry_at(registry, 0U);
        assert(strcmp(remaining->capability_id, "example.gamma") == 0);
        assert(strcmp(remaining->provider_module_id, "example.third") == 0);
    } else assert(!"Unknown capability case");
    umi_capability_registry_destroy(registry);
}

static void FillTo(UmiCapabilityRegistry *registry, size_t desired)
{
    for (size_t index = umi_capability_registry_count(registry); index < desired; ++index) {
        char name[64];
        int written = snprintf(name, sizeof(name), "example.capacity.%zu", index);
        assert(written > 0 && (size_t)written < sizeof(name));
        UmiCapabilityDescriptor value = Capability(name, "example.fill", 0U);
        assert(umi_capability_registry_register(registry, &value) == UMI_STATUS_OK);
    }
}
static void CheckCapacity(void)
{
    UmiCapabilityRegistry *registry = NULL;
    assert(umi_capability_registry_create(&registry) == UMI_STATUS_OK);
    FillTo(registry, UMI_CAPABILITY_REGISTRY_MAX - 1U);
    UmiCapabilityDescriptor pair[] = {
        Capability("example.one", "example.fill", 0U),
        Capability("example.two", "example.fill", 0U)
    };
    assert(UmiCapabilityRegistryRegisterBatch(registry, pair, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_capability_registry_count(registry) == UMI_CAPABILITY_REGISTRY_MAX - 1U);
    assert(umi_capability_registry_find(registry, "example.one") == NULL);
    assert(UmiCapabilityRegistryRegisterBatch(registry, pair, 1U) == UMI_STATUS_OK);
    umi_capability_registry_destroy(registry);
}

typedef struct Owner {
    UmiMasterController *master;
    UmiModuleDescriptor module;
    unsigned destroyed;
    unsigned diagnostics;
    int check_reentry;
} Owner;
static void Destroy(UmiModuleContext *context)
{
    Owner *owner = context->module_state;
    ++owner->destroyed;
}
static UmiModuleDescriptor Module(Owner *owner, const char *id)
{
    UmiModuleDescriptor module = {0};
    module.structure_size = (uint32_t)sizeof(module);
    module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    module.module_id = id;
    module.display_name = id;
    module.kind = UMI_MODULE_SERVICE;
    module.module_state = owner;
    module.lifecycle.destroy = Destroy;
    return module;
}
static void Diagnostic(const UmiDiagnostic *diagnostic, void *data)
{
    Owner *owner = data;
    if (!owner->check_reentry || owner->master == NULL ||
        strstr(diagnostic->message, "registered") == NULL) return;
    ++owner->diagnostics;
    UmiMasterController *borrowed = owner->master;
    assert(UmiMasterControllerRelease(&borrowed) == UMI_STATUS_INVALID_STATE);
    assert(borrowed == owner->master);
    assert(umi_master_controller_start(borrowed) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_validate(borrowed, NULL) == UMI_STATUS_INVALID_STATE);
    assert(umi_master_controller_register(borrowed, &owner->module) == UMI_STATUS_INVALID_STATE);
    umi_master_controller_destroy(borrowed);
    assert(umi_master_controller_module_count(borrowed) == 1U);
}
static void CheckMaster(const char *scenario)
{
    Owner owner = {0};
    UmiMasterControllerConfig config = {"Registration fixture", Diagnostic, &owner};
    assert(umi_master_controller_create(&config, &owner.master) == UMI_STATUS_OK);
    owner.module = Module(&owner, "example.module");
    UmiCapabilityRegistry *registry = umi_master_controller_capabilities(owner.master);
    size_t baseline = umi_capability_registry_count(registry);
    const char *invalid[] = {"example.first", "", NULL};
    const char *duplicate[] = {"example.first", "example.first", NULL};
    const char *corrected[] = {"example.first", "example.second", NULL};
    owner.module.provided_capabilities = corrected;
    if (strcmp(scenario, "master-callback") == 0) {
        owner.check_reentry = 1;
    } else {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(scenario, "master-invalid") == 0) owner.module.provided_capabilities = invalid;
        else if (strcmp(scenario, "master-duplicate") == 0) {
            owner.module.provided_capabilities = duplicate;
            expected = UMI_STATUS_ALREADY_EXISTS;
        } else if (strcmp(scenario, "master-capacity") == 0) {
            FillTo(registry, UMI_CAPABILITY_REGISTRY_MAX - 1U);
            baseline = umi_capability_registry_count(registry);
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else assert(!"Unknown master registration case");
        assert(umi_master_controller_register(owner.master, &owner.module) == expected);
        assert(umi_master_controller_module_count(owner.master) == 0U);
        assert(umi_capability_registry_count(registry) == baseline);
        assert(umi_capability_registry_find(registry, "example.first") == NULL);
        assert(owner.destroyed == 0U);
        if (strcmp(scenario, "master-capacity") == 0) {
            const UmiCapabilityDescriptor *last = umi_capability_registry_at(registry, baseline - 1U);
            assert(umi_capability_registry_unregister(registry,
                last->capability_id, last->provider_module_id) == UMI_STATUS_OK);
        }
        owner.module.provided_capabilities = corrected;
    }
    assert(umi_master_controller_register(owner.master, &owner.module) == UMI_STATUS_OK);
    assert(umi_master_controller_module_count(owner.master) == 1U);
    assert(umi_capability_registry_find(registry, "example.first") != NULL);
    if (owner.check_reentry) assert(owner.diagnostics == 1U);
    owner.check_reentry = 0;
    assert(UmiMasterControllerRelease(&owner.master) == UMI_STATUS_OK);
    assert(owner.master == NULL && owner.destroyed == 1U);
}

static void CheckMembership(void)
{
    Owner owner = {0};
    UmiModuleDescriptor modules[] = {
        Module(&owner, "example.one"), Module(&owner, "example.two"), Module(&owner, "example.three")
    };
    UmiModuleRegistry *registry = NULL;
    assert(umi_module_registry_create(&registry) == UMI_STATUS_OK);
    for (size_t index = 0U; index < 3U; ++index)
        assert(umi_module_registry_add(registry, &modules[index]) == UMI_STATUS_OK);
    assert(UmiModuleRegistryRemove(NULL, "example.one") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiModuleRegistryRemove(registry, "") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiModuleRegistryRemove(registry, "missing") == UMI_STATUS_NOT_FOUND);
    assert(UmiModuleRegistryRemove(registry, "example.two") == UMI_STATUS_OK);
    assert(umi_module_registry_count(registry) == 2U);
    assert(umi_module_registry_at(registry, 1U) == &modules[2]);
    assert(umi_module_registry_find(registry, "example.two") == NULL);
    assert(umi_module_registry_add(registry, &modules[1]) == UMI_STATUS_OK);
    assert(owner.destroyed == 0U);
    umi_module_registry_destroy(registry);
    assert(owner.destroyed == 0U);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "capacity") == 0) CheckCapacity();
    else if (strcmp(argv[1], "membership") == 0) CheckMembership();
    else if (strncmp(argv[1], "master-", 7U) == 0) CheckMaster(argv[1]);
    else CheckBatch(argv[1]);
    return 0;
}
