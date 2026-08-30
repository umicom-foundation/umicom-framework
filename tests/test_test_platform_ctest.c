/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_ctest.c
 *
 * PURPOSE:
 *   Verify CTest JSON v1 metadata import, suite creation and discovery records.
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

#include "umicom/test_platform/test_platform.h"

int main(void)
{
    static const char json[] =
        "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1},\"tests\":["
        "{\"name\":\"framework.alpha\","
        "\"command\":[\"C:/build/alpha.exe\",\"--verbose\"],"
        "\"properties\":["
        "{\"name\":\"LABELS\",\"value\":[\"unit\",\"fast\"]},"
        "{\"name\":\"TIMEOUT\",\"value\":30.0},"
        "{\"name\":\"WORKING_DIRECTORY\",\"value\":\"C:/build\"}]},"
        "{\"name\":\"framework.beta\","
        "\"command\":[\"C:/build/beta.exe\"],"
        "\"properties\":[{\"name\":\"DISABLED\",\"value\":true}]}]}";
    UmiTestPlatformService *service = NULL;
    UmiTestPlatformCtestImportOptions options;
    UmiTestPlatformCtestImportSummary summary;
    UmiTestPlatformItemSnapshot item;
    UmiTestPlatformSuiteSnapshot suite;
    UmiTestPlatformDiscoverySnapshot discovery;
    UmiTestPlatformServiceSnapshot snapshot;

    assert(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    (void)memset(&options, 0, sizeof(options));
    (void)strcpy(options.project_id, "framework");
    (void)strcpy(options.suite_id, "framework.ctest");
    (void)strcpy(options.configuration, "Debug");
    (void)strcpy(options.build_directory, "C:/build");
    assert(umi_test_platform_service_import_ctest_json(
               service, json, &options, &summary) == UMI_STATUS_OK);
    assert(summary.discovered_count == 2U);
    assert(summary.disabled_count == 1U);
    assert(summary.labelled_count == 1U);
    assert(summary.timed_count == 1U);
    assert(umi_test_platform_item_registry_find(
               umi_test_platform_service_item(service),
               "framework.ctest.framework.alpha", &item) == UMI_STATUS_OK);
    assert(strcmp(item.labels, "unit;fast") == 0);
    assert(strcmp(item.command, "C:/build/alpha.exe --verbose") == 0);
    assert(strcmp(item.working_directory, "C:/build") == 0);
    assert(item.timeout_ms == 30000U);
    assert(item.enabled == 1);
    assert(umi_test_platform_item_registry_find(
               umi_test_platform_service_item(service),
               "framework.ctest.framework.beta", &item) == UMI_STATUS_OK);
    assert(item.enabled == 0);
    assert(umi_test_platform_suite_registry_find(
               umi_test_platform_service_suite(service), "framework.ctest",
               &suite) == UMI_STATUS_OK);
    assert(suite.test_count == 2U);
    assert(umi_test_platform_discovery_registry_find(
               umi_test_platform_service_discovery(service),
               "discovery.framework.ctest", &discovery) == UMI_STATUS_OK);
    assert(discovery.metadata_supported == 1);
    assert(strcmp(discovery.configuration, "Debug") == 0);
    assert(umi_test_platform_service_snapshot(service, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.item_count == 3U);
    assert(snapshot.suite_count == 1U);
    assert(snapshot.discovery_count == 1U);
    umi_test_platform_service_destroy(service);
    return 0;
}
