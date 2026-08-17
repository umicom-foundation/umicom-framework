/*-----------------------------------------------------------------------------
 * Umicom Framework professional test provider registry tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/provider_registry.h"

static UmiTestPlatformProviderDescriptor provider(const char *id, int32_t priority,
                                                   uint32_t capabilities)
{
    UmiTestPlatformProviderDescriptor value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_TEST_PLATFORM_PROVIDER_API_VERSION;
    (void)strncpy(value.id, id, sizeof(value.id) - 1U);
    (void)strncpy(value.label, id, sizeof(value.label) - 1U);
    (void)strncpy(value.frameworks, "ctest;c;*", sizeof(value.frameworks) - 1U);
    value.kind = UMI_TEST_PLATFORM_PROVIDER_CTEST;
    value.priority = priority;
    value.capabilities = capabilities;
    value.enabled = 1;
    value.trusted = 1;
    value.available = 1;
    return value;
}

int main(void)
{
    UmiTestPlatformProviderRegistry *registry = NULL;
    UmiTestPlatformProviderDescriptor first = provider(
        "provider.a", 10, UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DISCOVERY |
                          UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION);
    UmiTestPlatformProviderDescriptor second = provider(
        "provider.b", 20, UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_DISCOVERY |
                          UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION |
                          UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_COVERAGE);
    UmiTestPlatformProviderQuery query;
    UmiTestPlatformProviderSelection selection;
    assert(umi_test_platform_provider_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_platform_provider_registry_upsert(registry, &first) == UMI_STATUS_OK);
    assert(umi_test_platform_provider_registry_upsert(registry, &second) == UMI_STATUS_OK);
    umi_test_platform_provider_query_init(&query);
    query.framework = "ctest";
    query.required_capabilities = UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_EXECUTION |
                                  UMI_TEST_PLATFORM_PROVIDER_CAPABILITY_COVERAGE;
    assert(umi_test_platform_provider_registry_select(registry, &query,
                                                       &selection) == UMI_STATUS_OK);
    assert(selection.found && strcmp(selection.provider.id, "provider.b") == 0);
    assert(umi_test_platform_provider_registry_remove(registry, "provider.b") == UMI_STATUS_OK);
    assert(umi_test_platform_provider_registry_select(registry, &query,
                                                       &selection) == UMI_STATUS_NOT_FOUND);
    umi_test_platform_provider_registry_destroy(registry);
    return 0;
}
