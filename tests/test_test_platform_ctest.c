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
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "umicom/test_platform/test_platform.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* No configured tree or CTest executable is needed to verify execution origin.
 * Keep this check ahead of process submission in every frontend. */
static void TestSelectionOrigin(void)
{
#ifdef _WIN32
    const char *root = "C:/Projects/Notes/build";
    const char *equivalent = "C:/Projects/Notes/./build";
    const char *otherRoot = "C:/Projects/Other/build";
#else
    const char *root = "/projects/Notes/build";
    const char *equivalent = "/projects/Notes/./build";
    const char *otherRoot = "/projects/Other/build";
#endif
    const char *json = "{\"tests\":[{\"name\":\"smoke\",\"command\":[\"unused\"]}]}";
    UmiTestPlatformService *service = NULL;
    UmiTestPlatformSelection *selection = calloc(1U, sizeof *selection);
    UmiTestPlatformSelection *before = calloc(1U, sizeof *before);
    assert(selection != NULL && before != NULL);
    assert(umi_test_platform_service_create(&service) == UMI_STATUS_OK);
    UmiTestPlatformCtestImportOptions options = {0};
    strcpy(options.project_id, "selected");
    strcpy(options.suite_id, "selected.ctest");
    strcpy(options.build_directory, root);
    strcpy(options.configuration, "Debug");
    UmiTestPlatformCtestImportSummary summary;
    assert(umi_test_platform_service_import_ctest_json(service, json, &options, &summary) == UMI_STATUS_OK);
    UmiTestPlatformItemRegistry *items = umi_test_platform_service_item(service);
    UmiTestPlatformDiscoveryRegistry *discoveries = umi_test_platform_service_discovery(service);
    selection->count = 1U;
    strcpy(selection->item_ids[0], "selected.ctest.smoke");
    selection->source_revision = umi_test_platform_item_registry_revision(items);
    *before = *selection;
    uint64_t itemRevision = umi_test_platform_item_registry_revision(items);
    uint64_t discoveryRevision = umi_test_platform_discovery_registry_revision(discoveries);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_OK);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", equivalent, "") == UMI_STATUS_OK);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "other", root, "Debug") == UMI_STATUS_INVALID_STATE);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", otherRoot, "Debug") == UMI_STATUS_INVALID_STATE);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Release") == UMI_STATUS_INVALID_STATE);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", "relative", "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiTestPlatformCtestCheckSelection(NULL, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiTestPlatformCtestCheckSelection(items, NULL, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, NULL, "selected", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(selection, before, sizeof *before) == 0);
    assert(umi_test_platform_item_registry_revision(items) == itemRevision);
    assert(umi_test_platform_discovery_registry_revision(discoveries) == discoveryRevision);
    selection->count = 0U;
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    selection->count = sizeof selection->item_ids / sizeof selection->item_ids[0] + 1U;
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_CAPACITY_EXCEEDED);
    *selection = *before;
    memset(selection->item_ids[0], 'x', sizeof selection->item_ids[0]);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(selection->item_ids[0], "not.discovered");
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_NOT_FOUND);
    *selection = *before;
    /* A later row from another project must reject the whole selection. */
    strcpy(options.project_id, "other");
    strcpy(options.suite_id, "other.ctest");
    strcpy(options.build_directory, otherRoot);
    assert(umi_test_platform_service_import_ctest_json(service, json, &options, &summary) == UMI_STATUS_OK);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_STATE);
    selection->source_revision = umi_test_platform_item_registry_revision(items);
    selection->count = 2U;
    strcpy(selection->item_ids[1], "other.ctest.smoke");
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_STATE);
    selection->count = 1U;
    UmiTestPlatformDiscoverySnapshot discovery;
    assert(umi_test_platform_discovery_registry_find(discoveries, "discovery.selected.ctest", &discovery) == UMI_STATUS_OK);
    strcpy(discovery.project_id, "other");
    assert(umi_test_platform_discovery_registry_upsert(discoveries, &discovery) == UMI_STATUS_OK);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_INVALID_STATE);
    assert(umi_test_platform_discovery_registry_remove(discoveries, "discovery.selected.ctest") == UMI_STATUS_OK);
    assert(UmiTestPlatformCtestCheckSelection(items, discoveries, selection, "selected", root, "Debug") == UMI_STATUS_NOT_FOUND);
    umi_test_platform_service_destroy(service);
    free(before);
    free(selection);
}

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
    TestSelectionOrigin();
    return 0;
}
