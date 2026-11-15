/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_provider_registry_lifetime.c
 *
 * PURPOSE:
 *   Exercise bounded provider registry validation, ownership retention and callback reentry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ai/provider_registry.h"

typedef struct Owner {
    UmiAiProviderRegistry *registry;
    unsigned *order;
    size_t *used;
    unsigned number;
    int reenter;
} Owner;

static UmiStatus Generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)instance; (void)request; (void)response;
    return UMI_STATUS_OK;
}
static UmiAiProvider Provider(const char *id)
{
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = id;
    provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.generate = Generate;
    return provider;
}
static void Destroy(void *instance)
{
    Owner *owner = instance;
    owner->order[(*owner->used)++] = owner->number;
    if (owner->reenter) {
        UmiAiProvider replacement = Provider("replacement");
        assert(UmiAiProviderRegistryRelease(owner->registry) == UMI_STATUS_INVALID_STATE);
        assert(umi_ai_provider_registry_find(owner->registry, "one") == NULL);
        assert(umi_ai_provider_registry_add(owner->registry, &replacement) == UMI_STATUS_INVALID_STATE);
        umi_ai_provider_registry_destroy(owner->registry);
        assert(owner->registry->count > UMI_AI_MAX_PROVIDERS);
    }
}
static void InvalidStorage(unsigned fault)
{
    UmiAiProviderRegistry registry, before, valid;
    unsigned order[2] = {0};
    size_t used = 0U;
    Owner owners[2] = {{&registry, order, &used, 1U, 0}, {&registry, order, &used, 2U, 0}};
    umi_ai_provider_registry_init(&registry);
    for (size_t i = 0U; i < 2U; ++i) {
        UmiAiProvider provider = Provider(i == 0U ? "one" : "two");
        provider.instance = &owners[i]; provider.destroy = Destroy;
        assert(umi_ai_provider_registry_add(&registry, &provider) == UMI_STATUS_OK);
    }
    memcpy(&valid, &registry, sizeof(valid));
    switch (fault) {
        case 0U: registry.count = SIZE_MAX; break;
        case 1U: registry.providers[1].provider_id = NULL; break;
        case 2U: registry.providers[1].generate = NULL; break;
        case 3U: registry.providers[1].kind = (UmiAiProviderKind)99; break;
        case 4U: registry.providers[1].abi_version = 9U; break;
        case 5U: registry.providers[1].structure_size = 0U; break;
        case 6U: registry.providers[1].provider_id = "one"; break;
        default: assert(0); return;
    }
    memcpy(&before, &registry, sizeof(before));
    UmiAiProvider candidate = Provider("three");
    assert(UmiAiProviderRegistryValidate(&registry) == UMI_STATUS_INVALID_STATE);
    assert(umi_ai_provider_registry_find(&registry, "one") == NULL);
    assert(umi_ai_provider_registry_add(&registry, &candidate) == UMI_STATUS_INVALID_STATE);
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_INVALID_STATE);
    umi_ai_provider_registry_destroy(&registry);
    assert(used == 0U && memcmp(&before, &registry, sizeof(before)) == 0);
    /* The fixture restores the known original descriptor, not production
     * recovery logic. The real owner must diagnose corruption before cleanup. */
    memcpy(&registry, &valid, sizeof(registry));
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_OK);
    assert(used == 2U && order[0] == 2U && order[1] == 1U);
}
static void ReentrantRelease(void)
{
    UmiAiProviderRegistry registry;
    unsigned order[2] = {0};
    size_t used = 0U;
    Owner owners[2] = {{&registry, order, &used, 1U, 1}, {&registry, order, &used, 2U, 1}};
    umi_ai_provider_registry_init(&registry);
    for (size_t i = 0U; i < 2U; ++i) {
        UmiAiProvider provider = Provider(i == 0U ? "one" : "two");
        provider.instance = &owners[i]; provider.destroy = Destroy;
        assert(umi_ai_provider_registry_add(&registry, &provider) == UMI_STATUS_OK);
    }
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_OK);
    assert(registry.count == 0U && used == 2U && order[0] == 2U && order[1] == 1U);
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_OK);
    assert(used == 2U);
}
static void CapacityAndBorrowing(void)
{
    UmiAiProviderRegistry registry, before;
    char ids[UMI_AI_MAX_PROVIDERS][32];
    umi_ai_provider_registry_init(&registry);
    for (size_t i = 0U; i < UMI_AI_MAX_PROVIDERS; ++i) {
        int written = snprintf(ids[i], sizeof(ids[i]), "provider-%zu", i);
        assert(written > 0 && (size_t)written < sizeof(ids[i]));
        UmiAiProvider provider = Provider(ids[i]);
        assert(umi_ai_provider_registry_add(&registry, &provider) == UMI_STATUS_OK);
        assert(registry.providers[i].provider_id == ids[i]);
    }
    memcpy(&before, &registry, sizeof(before));
    UmiAiProvider duplicate = Provider(ids[0]), overflow = Provider("overflow");
    assert(umi_ai_provider_registry_add(&registry, &duplicate) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_ai_provider_registry_add(&registry, &overflow) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&before, &registry, sizeof(before)) == 0);
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_OK);
    /* The registry has never imposed request-record ID limits on borrowed
     * provider names. Keep that compatibility boundary explicit. */
    char long_id[UMI_AI_ID_CAPACITY * 2U];
    memset(long_id, 'x', sizeof(long_id) - 1U);
    long_id[sizeof(long_id) - 1U] = '\0';
    UmiAiProvider provider = Provider(long_id);
    assert(umi_ai_provider_registry_add(&registry, &provider) == UMI_STATUS_OK);
    assert(umi_ai_provider_registry_find(&registry, long_id) == &registry.providers[0]);
    assert(UmiAiProviderRegistryRelease(&registry) == UMI_STATUS_OK);
}
int main(void)
{
    assert(UmiAiProviderRegistryValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiAiProviderRegistryRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_provider_registry_find(NULL, "x") == NULL);
    umi_ai_provider_registry_destroy(NULL);
    for (unsigned fault = 0U; fault < 7U; ++fault) InvalidStorage(fault);
    ReentrantRelease();
    CapacityAndBorrowing();
    return 0;
}
