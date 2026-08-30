/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_secret_provider_mutation.c
 *
 * PURPOSE:
 *   Verify provider-qualified secret references support safe store, resolve,
 *   remove and in-memory clearing without placing a secret in settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/umicom.h"

typedef struct TestSecretStore {
    char name[64];
    char value[128];
} TestSecretStore;

static UmiStatus test_get(void *instance,
                          const char *name,
                          char *out_value,
                          size_t capacity)
{
    TestSecretStore *store = (TestSecretStore *)instance;
    size_t length;
    if (strcmp(store->name, name) != 0) return UMI_STATUS_NOT_FOUND;
    length = strlen(store->value);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_value, store->value, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus test_set(void *instance,
                          const char *name,
                          const char *value)
{
    TestSecretStore *store = (TestSecretStore *)instance;
    if (strlen(name) >= sizeof(store->name) ||
        strlen(value) >= sizeof(store->value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strcpy(store->name, name);
    (void)strcpy(store->value, value);
    return UMI_STATUS_OK;
}

static UmiStatus test_remove(void *instance, const char *name)
{
    TestSecretStore *store = (TestSecretStore *)instance;
    if (strcmp(store->name, name) != 0) return UMI_STATUS_NOT_FOUND;
    umi_secret_clear(store->value, sizeof(store->value));
    store->name[0] = '\0';
    return UMI_STATUS_OK;
}

int main(void)
{
    TestSecretStore store = {{0}, {0}};
    UmiSecretProvider provider = {0};
    UmiSecretProviderRegistry *registry = NULL;
    char value[128];

    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    provider.instance = &store;
    provider.get = test_get;
    provider.set = test_set;
    provider.remove = test_remove;
    assert(umi_secret_provider_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_secret_provider_registry_add(registry, "test-vault", &provider)
           == UMI_STATUS_OK);
    assert(umi_secret_provider_registry_store(
        registry, "test-vault://ai/example", "local-secret") ==
        UMI_STATUS_OK);
    assert(umi_secret_provider_registry_resolve(
        registry, "test-vault://ai/example", value, sizeof(value)) ==
        UMI_STATUS_OK);
    assert(strcmp(value, "local-secret") == 0);
    umi_secret_clear(value, sizeof(value));
    assert(value[0] == '\0');
    assert(umi_secret_provider_registry_remove(
        registry, "test-vault://ai/example") == UMI_STATUS_OK);
    assert(umi_secret_provider_registry_resolve(
        registry, "test-vault://ai/example", value, sizeof(value)) ==
        UMI_STATUS_NOT_FOUND);
    umi_secret_provider_registry_destroy(registry);
    return 0;
}
