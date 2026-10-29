/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_service_registry_ownership.c
 *
 * PURPOSE:
 *   Verify service replacement, relocation and destructor reentry ownership.
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
#include "umicom/runtime/service_registry.h"
#include <assert.h>
#include <string.h>

typedef struct Fixture Fixture;
typedef struct Object { Fixture *fixture; unsigned index; } Object;
struct Fixture {
    UmiServiceRegistry *registry;
    UmiMasterController *master;
    Object objects[4];
    unsigned destroyed[4];
    unsigned order[8];
    unsigned count;
    int check_callback;
    unsigned module_destroyed;
};
static UmiServiceDescriptor Describe(Object *object, const char *id,
                                     const char *provider, uint32_t flags);

static void Destroy(void *data, void *user_data)
{
    Object *object = data;
    Fixture *fixture = user_data;
    assert(object->fixture == fixture);
    assert(fixture->count < 8U);
    fixture->order[fixture->count++] = object->index;
    ++fixture->destroyed[object->index];
    if (!fixture->check_callback) return;
    assert(UmiServiceRegistryIsInCallback(fixture->registry));
    UmiServiceRegistry *borrowed = fixture->registry;
    assert(UmiServiceRegistryRelease(&borrowed) == UMI_STATUS_INVALID_STATE);
    assert(borrowed == fixture->registry);
    UmiServiceDescriptor candidate = Describe(&fixture->objects[3],
        "example.callback", "example.provider", 0U);
    assert(umi_service_registry_register(borrowed, &candidate) == UMI_STATUS_INVALID_STATE);
    assert(umi_service_registry_unregister(borrowed, "missing", "missing")
           == UMI_STATUS_INVALID_STATE);
    umi_service_registry_destroy(borrowed);
    /* Reentry must not destroy the catalogue even through the legacy API. */
    assert(umi_service_registry_count(borrowed) <= UMI_SERVICE_REGISTRY_MAX);
    if (fixture->master != NULL) {
        UmiMasterController *owner = fixture->master;
        assert(UmiMasterControllerRelease(&owner) == UMI_STATUS_INVALID_STATE);
        assert(owner == fixture->master);
        assert(umi_master_controller_start(owner) == UMI_STATUS_INVALID_STATE);
        assert(umi_master_controller_validate(owner, NULL) == UMI_STATUS_INVALID_STATE);
        umi_master_controller_destroy(owner);
        assert(fixture->module_destroyed == 0U);
    }
}
static UmiServiceDescriptor Describe(Object *object, const char *id,
                                     const char *provider, uint32_t flags)
{
    UmiServiceDescriptor descriptor = {0};
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = id;
    descriptor.service_version = (UmiVersion){1U, 0U, 0U};
    descriptor.provider_module_id = provider;
    descriptor.service = object;
    descriptor.destroy = Destroy;
    descriptor.destroy_user_data = object->fixture;
    descriptor.flags = flags;
    return descriptor;
}
static void Prepare(Fixture *fixture)
{
    memset(fixture, 0, sizeof(*fixture));
    for (unsigned index = 0U; index < 4U; ++index) {
        fixture->objects[index].fixture = fixture;
        fixture->objects[index].index = index;
    }
    assert(umi_service_registry_create(&fixture->registry) == UMI_STATUS_OK);
}
static void Close(Fixture *fixture)
{
    assert(!UmiServiceRegistryIsInCallback(fixture->registry));
    assert(UmiServiceRegistryRelease(&fixture->registry) == UMI_STATUS_OK);
    assert(fixture->registry == NULL);
}
static void CheckReplacement(const char *scenario)
{
    Fixture fixture;
    Prepare(&fixture);
    uint32_t flags = UMI_SERVICE_OWNED | UMI_SERVICE_REPLACEABLE;
    UmiServiceDescriptor first = Describe(&fixture.objects[0], "example.service",
                                          "example.provider", flags);
    assert(umi_service_registry_register(fixture.registry, &first) == UMI_STATUS_OK);
    UmiServiceDescriptor second = first;
    if (strcmp(scenario, "same-object") == 0) {
        const UmiServiceDescriptor *borrowed = umi_service_registry_find(
            fixture.registry, "example.service");
        assert(umi_service_registry_register(fixture.registry, borrowed) == UMI_STATUS_OK);
        assert(fixture.destroyed[0] == 0U);
    } else if (strcmp(scenario, "metadata-only") == 0) {
        second.flags = UMI_SERVICE_NONE;
        assert(umi_service_registry_register(fixture.registry, &second) == UMI_STATUS_OK);
        assert(fixture.destroyed[0] == 0U);
        Close(&fixture);
        assert(fixture.destroyed[0] == 0U);
        return;
    } else if (strcmp(scenario, "invalid-replacement") == 0) {
        char oversized[UMI_SERVICE_ID_CAPACITY + 1U];
        memset(oversized, 'z', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        second.service_id = oversized;
        assert(umi_service_registry_register(fixture.registry, &second)
               == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(fixture.destroyed[0] == 0U);
    } else {
        second.service = &fixture.objects[1];
        fixture.check_callback = strcmp(scenario, "replace-callback") == 0;
        /* Borrowed identifier strings must survive overwriting their old entry. */
        const UmiServiceDescriptor *borrowed = umi_service_registry_at(fixture.registry, 0U);
        second.service_id = borrowed->service_id;
        second.provider_module_id = borrowed->provider_module_id;
        assert(umi_service_registry_register(fixture.registry, &second) == UMI_STATUS_OK);
        assert(fixture.destroyed[0] == 1U);
        const UmiServiceDescriptor *found = umi_service_registry_find(
            fixture.registry, "example.service");
        assert(found != NULL && found->service == &fixture.objects[1]);
        assert(strcmp(found->provider_module_id, "example.provider") == 0);
    }
    Close(&fixture);
    assert(fixture.destroyed[0] == 1U);
    if (second.service == &fixture.objects[1]) assert(fixture.destroyed[1] == 1U);
}
static void CheckSingleton(const char *scenario)
{
    Fixture fixture;
    Prepare(&fixture);
    UmiServiceDescriptor first = Describe(&fixture.objects[0], "example.service",
        "example.first", UMI_SERVICE_OWNED | UMI_SERVICE_REPLACEABLE);
    UmiServiceDescriptor second = Describe(&fixture.objects[1], "example.service",
        "example.second", UMI_SERVICE_OWNED);
    if (strcmp(scenario, "singleton-existing") == 0) first.flags |= UMI_SERVICE_SINGLETON;
    if (strcmp(scenario, "singleton-incoming") == 0) second.flags |= UMI_SERVICE_SINGLETON;
    assert(umi_service_registry_register(fixture.registry, &first) == UMI_STATUS_OK);
    if (strcmp(scenario, "singleton-promotion") == 0) {
        assert(umi_service_registry_register(fixture.registry, &second) == UMI_STATUS_OK);
        first.flags |= UMI_SERVICE_SINGLETON;
        assert(umi_service_registry_register(fixture.registry, &first) == UMI_STATUS_ALREADY_EXISTS);
        assert(umi_service_registry_count(fixture.registry) == 2U);
        assert(fixture.destroyed[0] == 0U && fixture.destroyed[1] == 0U);
        Close(&fixture);
        assert(fixture.destroyed[1] == 1U);
    } else {
        assert(umi_service_registry_register(fixture.registry, &second) == UMI_STATUS_ALREADY_EXISTS);
        Close(&fixture);
        assert(fixture.destroyed[1] == 0U);
    }
    assert(fixture.destroyed[0] == 1U);
}
static void CheckCompaction(void)
{
    Fixture fixture;
    Prepare(&fixture);
    const char *ids[] = {"example.one", "example.two", "example.three", "example.four"};
    for (unsigned index = 0U; index < 3U; ++index) {
        UmiServiceDescriptor value = Describe(&fixture.objects[index], ids[index],
                                             ids[index], UMI_SERVICE_OWNED);
        assert(umi_service_registry_register(fixture.registry, &value) == UMI_STATUS_OK);
    }
    assert(umi_service_registry_unregister(fixture.registry, ids[0], ids[0]) == UMI_STATUS_OK);
    UmiServiceDescriptor last = Describe(&fixture.objects[3], ids[3], ids[3], UMI_SERVICE_OWNED);
    assert(umi_service_registry_register(fixture.registry, &last) == UMI_STATUS_OK);
    for (unsigned index = 1U; index < 4U; ++index) {
        const UmiServiceDescriptor *found = umi_service_registry_find(fixture.registry, ids[index]);
        assert(found != NULL && found->service == &fixture.objects[index]);
        assert(strcmp(found->service_id, ids[index]) == 0);
        assert(strcmp(found->provider_module_id, ids[index]) == 0);
    }
    Close(&fixture);
    assert(fixture.count == 4U);
    assert(fixture.order[0] == 0U && fixture.order[1] == 3U &&
           fixture.order[2] == 2U && fixture.order[3] == 1U);
    for (unsigned index = 0U; index < 4U; ++index) assert(fixture.destroyed[index] == 1U);
}
static void ModuleDestroy(UmiModuleContext *context)
{
    Fixture *fixture = context->module_state;
    ++fixture->module_destroyed;
}
static void CheckCallback(const char *scenario)
{
    Fixture fixture;
    Prepare(&fixture);
    UmiModuleDescriptor module = {0};
    if (strcmp(scenario, "master-callback") == 0) {
        Close(&fixture);
        UmiMasterControllerConfig config = {"Owned services", NULL, NULL};
        assert(umi_master_controller_create(&config, &fixture.master) == UMI_STATUS_OK);
        fixture.registry = umi_master_controller_services(fixture.master);
        module.structure_size = (uint32_t)sizeof(module);
        module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
        module.module_id = "example.module";
        module.display_name = "Example module";
        module.kind = UMI_MODULE_SERVICE;
        module.module_state = &fixture;
        module.lifecycle.destroy = ModuleDestroy;
        assert(umi_master_controller_register(fixture.master, &module) == UMI_STATUS_OK);
    }
    UmiServiceDescriptor owned = Describe(&fixture.objects[0], "example.owned",
                                         "example.provider", UMI_SERVICE_OWNED);
    assert(umi_service_registry_register(fixture.registry, &owned) == UMI_STATUS_OK);
    fixture.check_callback = 1;
    if (strcmp(scenario, "release-callback") == 0) Close(&fixture);
    else {
        assert(umi_service_registry_unregister(fixture.registry, "example.owned", "example.provider")
               == UMI_STATUS_OK);
        assert(umi_service_registry_find(fixture.registry, "example.owned") == NULL);
        assert(!UmiServiceRegistryIsInCallback(fixture.registry));
        fixture.check_callback = 0;
        if (fixture.master != NULL) {
            assert(UmiMasterControllerRelease(&fixture.master) == UMI_STATUS_OK);
            fixture.registry = NULL; /* The controller owned the registry. */
            assert(fixture.module_destroyed == 1U);
        } else Close(&fixture);
    }
    assert(fixture.destroyed[0] == 1U);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "empty") == 0) {
        UmiServiceRegistry *empty = NULL;
        assert(UmiServiceRegistryRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiServiceRegistryRelease(&empty) == UMI_STATUS_OK);
        assert(!UmiServiceRegistryIsInCallback(NULL));
    } else if (strcmp(argv[1], "compaction") == 0) CheckCompaction();
    else if (strncmp(argv[1], "singleton-", 10U) == 0) CheckSingleton(argv[1]);
    else if (strcmp(argv[1], "unregister-callback") == 0 ||
             strcmp(argv[1], "release-callback") == 0 ||
             strcmp(argv[1], "master-callback") == 0) CheckCallback(argv[1]);
    else if (strcmp(argv[1], "same-object") == 0 ||
             strcmp(argv[1], "metadata-only") == 0 ||
             strcmp(argv[1], "invalid-replacement") == 0 ||
             strcmp(argv[1], "owned-replacement") == 0 ||
             strcmp(argv[1], "replace-callback") == 0) CheckReplacement(argv[1]);
    else assert(!"Unknown service ownership case");
    return 0;
}
