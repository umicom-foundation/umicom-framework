/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest.c
 *
 * PURPOSE:
 *   Parse CTest JSON v1 discovery records into portable test-platform data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/ctest.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/process.h"

/* Migration: the previous provider mutated live registries while parsing and
 * consumed only the fixed process tail. Its public entry points remain below.
 * The stricter private reader replaces that implementation, preserving names,
 * labels, disabled state, timeout, command display and working-directory data.
 * Source locations are additionally projected from CTest's backtrace graph.
 */
#include "ctest_json_reader.inc"
#include "ctest_json_metadata.inc"

static UmiStatus UmiCtestImportJson(const char *json, size_t length,
    const UmiTestPlatformCtestImportOptions *options, size_t maximumTests, int strictEnvelope,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries, UmiTestPlatformCtestImportSummary *outSummary)
{
    UmiStatus status;
    UmiTestPlatformItemRegistry *stagedItems = NULL;
    UmiTestPlatformSuiteRegistry *stagedSuites = NULL;
    UmiTestPlatformDiscoveryRegistry *stagedDiscoveries = NULL;
    UmiTestPlatformCtestImportSummary summary = {0U, 0U, 0U, 0U};
    UmiCtestJsonArray tests = {NULL, 0U, 0U};
    UmiCtestSourceGraph graph = {{NULL, 0U, 0U}, {NULL, 0U, 0U}};
    UmiCtestJsonSpan document, value, kind, version, component;
    UmiTestPlatformItemSnapshot root, existingItem;
    UmiTestPlatformSuiteSnapshot suite, existingSuite;
    UmiTestPlatformDiscoverySnapshot discovery, existingDiscovery;
    size_t index, number;
    char type[32];
    if (outSummary != NULL) (void)memset(outSummary, 0, sizeof(*outSummary));
    status = UmiCtestOptionsValidate(options);
    if (status != UMI_STATUS_OK) return status;
    if (json == NULL || items == NULL || suites == NULL || discoveries == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_TEST_PLATFORM_CTEST_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(json, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiCtestJsonReader reader = {json, json + length, UMI_STATUS_OK};
    if (!UmiCtestJsonValue(&reader, 0U)) return reader.status;
    UmiCtestJsonSpace(&reader);
    if (reader.cursor != reader.end) return UMI_STATUS_PARSE_ERROR;
    document.begin = json; document.end = json + length;
#define UMI_CTEST_TRY(expression) do { status = (expression); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    UMI_CTEST_TRY(UmiCtestJsonField(document, "kind", &kind));
    UMI_CTEST_TRY(UmiCtestJsonField(document, "version", &version));
    if (strictEnvelope || kind.begin != NULL || version.begin != NULL) {
        UMI_CTEST_TRY(UmiCtestJsonText(kind, type, sizeof(type)));
        if (strcmp(type, "ctestInfo") != 0 || version.begin == NULL) { status = UMI_STATUS_PARSE_ERROR; goto cleanup; }
        UMI_CTEST_TRY(UmiCtestJsonField(version, "major", &component));
        UMI_CTEST_TRY(UmiCtestJsonUnsigned(component, UINT32_MAX, &number));
        if (number != 1U) { status = UMI_STATUS_NOT_IMPLEMENTED; goto cleanup; }
        UMI_CTEST_TRY(UmiCtestJsonField(version, "minor", &component));
        /* Historical in-memory fixtures specify major only. Keep that legacy
         * entry point compatible; real process data and sized imports require
         * the complete version envelope from the CTest protocol. */
        if (component.begin != NULL || strictEnvelope) {
            UMI_CTEST_TRY(UmiCtestJsonUnsigned(component, UINT32_MAX, &number));
            if (number != 0U) { status = UMI_STATUS_NOT_IMPLEMENTED; goto cleanup; }
        }
    }
    UMI_CTEST_TRY(UmiCtestJsonField(document, "tests", &value));
    UMI_CTEST_TRY(UmiCtestJsonArrayRead(value, maximumTests, &tests));
    UMI_CTEST_TRY(UmiCtestJsonField(document, "backtraceGraph", &value));
    if (value.begin != NULL) {
        UMI_CTEST_TRY(UmiCtestJsonField(value, "files", &component));
        UMI_CTEST_TRY(UmiCtestJsonArrayRead(component, 65536U, &graph.files));
        UMI_CTEST_TRY(UmiCtestJsonField(value, "nodes", &component));
        UMI_CTEST_TRY(UmiCtestJsonArrayRead(component, 262144U, &graph.nodes));
    }
    /* A suite identifier is owned by one project. Reusing it for another
     * project would turn refresh into deletion of somebody else's catalogue. */
    status = umi_test_platform_suite_registry_find(suites, options->suite_id, &existingSuite);
    if (status == UMI_STATUS_OK && strcmp(existingSuite.project_id, options->project_id) != 0) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) goto cleanup;
    (void)memset(&discovery, 0, sizeof(discovery));
    (void)memcpy(discovery.id, "discovery.", sizeof("discovery.") - 1U);
    (void)memcpy(discovery.id + sizeof("discovery.") - 1U, options->suite_id, strlen(options->suite_id) + 1U);
    status = umi_test_platform_discovery_registry_find(discoveries, discovery.id, &existingDiscovery);
    if (status == UMI_STATUS_OK && strcmp(existingDiscovery.project_id, options->project_id) != 0) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) goto cleanup;
    UMI_CTEST_TRY(UmiTestPlatformItemRegistryClone(items, &stagedItems));
    UMI_CTEST_TRY(UmiTestPlatformSuiteRegistryClone(suites, &stagedSuites));
    UMI_CTEST_TRY(UmiTestPlatformDiscoveryRegistryClone(discoveries, &stagedDiscoveries));
    UMI_CTEST_TRY(UmiTestPlatformItemRegistryRemoveSuite(stagedItems, options->suite_id));
    status = umi_test_platform_item_registry_find(stagedItems, options->suite_id, &existingItem);
    if (status == UMI_STATUS_OK) { status = UMI_STATUS_ALREADY_EXISTS; goto cleanup; }
    if (status != UMI_STATUS_NOT_FOUND) goto cleanup;
    (void)memset(&root, 0, sizeof(root));
    (void)memcpy(root.id, options->suite_id, strlen(options->suite_id) + 1U);
    (void)memcpy(root.suite_id, options->suite_id, strlen(options->suite_id) + 1U);
    (void)memcpy(root.name, "CTest", sizeof("CTest"));
    (void)memcpy(root.uri, options->build_directory, strlen(options->build_directory) + 1U);
    (void)memcpy(root.framework, "ctest", sizeof("ctest"));
    (void)memcpy(root.kind, "suite", sizeof("suite"));
    root.enabled = 1; root.discovered = 1;
    UMI_CTEST_TRY(umi_test_platform_item_registry_upsert(stagedItems, &root));
    for (index = 0U; index < tests.count; ++index)
        UMI_CTEST_TRY(UmiCtestImportTest(tests.values[index], options, &graph, stagedItems, &summary));
    (void)memset(&suite, 0, sizeof(suite));
    (void)memcpy(suite.id, options->suite_id, strlen(options->suite_id) + 1U);
    (void)memcpy(suite.name, "CTest", sizeof("CTest"));
    (void)memcpy(suite.project_id, options->project_id, strlen(options->project_id) + 1U);
    (void)memcpy(suite.root_item_id, options->suite_id, strlen(options->suite_id) + 1U);
    suite.test_count = summary.discovered_count; suite.enabled = 1;
    UMI_CTEST_TRY(umi_test_platform_suite_registry_upsert(stagedSuites, &suite));
    (void)memcpy(discovery.project_id, options->project_id, strlen(options->project_id) + 1U);
    (void)memcpy(discovery.provider, "ctest-json-v1", sizeof("ctest-json-v1"));
    (void)memcpy(discovery.root_uri, options->build_directory, strlen(options->build_directory) + 1U);
    (void)memcpy(discovery.configuration, options->configuration, strlen(options->configuration) + 1U);
    discovery.discovered_count = summary.discovered_count;
    discovery.state = 1; discovery.metadata_supported = 1;
    UMI_CTEST_TRY(umi_test_platform_discovery_registry_upsert(stagedDiscoveries, &discovery));
    /* No allocation, error return or callback may be introduced between these
     * swaps. The owner serialises access to the three registries for this call. */
    UmiTestPlatformItemRegistrySwap(items, stagedItems);
    UmiTestPlatformSuiteRegistrySwap(suites, stagedSuites);
    UmiTestPlatformDiscoveryRegistrySwap(discoveries, stagedDiscoveries);
    if (outSummary != NULL) *outSummary = summary;
    status = UMI_STATUS_OK;
cleanup:
    umi_test_platform_item_registry_destroy(stagedItems);
    umi_test_platform_suite_registry_destroy(stagedSuites);
    umi_test_platform_discovery_registry_destroy(stagedDiscoveries);
    free(tests.values); free(graph.files.values); free(graph.nodes.values);
    return status;
#undef UMI_CTEST_TRY
}

/* Provide the test platform ctest parse json operation used by this module and
 * its client applications. The envelope-free form is retained for old callers. */
UmiStatus umi_test_platform_ctest_parse_json(const char *json,
    const UmiTestPlatformCtestImportOptions *options, UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites, UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *outSummary)
{
    return UmiCtestImportJson(json, json != NULL ? strlen(json) : 0U, options,
        UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY - 1U, 0, items, suites, discoveries, outSummary);
}

UmiStatus UmiTestPlatformCtestParseJsonSized(const char *json, size_t length,
    const UmiTestPlatformCtestImportOptions *options, UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites, UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *outSummary)
{
    return UmiCtestImportJson(json, length, options, UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY - 1U,
        1, items, suites, discoveries, outSummary);
}

#include "ctest_json_capture.inc"

/* The background path composes the same parser/capture implementation. */
#include "ctest_background.inc"

/* Resolve execution origin through the same metadata provider that discovered it. */
UmiStatus UmiTestPlatformCtestMakeRunRequest(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiCtestJobRequest *outRequest)
{
    if (item == NULL || discoveries == NULL || outRequest == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->id, '\0', sizeof(item->id)) == NULL ||
        memchr(item->name, '\0', sizeof(item->name)) == NULL ||
        memchr(item->suite_id, '\0', sizeof(item->suite_id)) == NULL ||
        memchr(item->framework, '\0', sizeof(item->framework)) == NULL ||
        memchr(item->kind, '\0', sizeof(item->kind)) == NULL ||
        memchr(item->uri, '\0', sizeof(item->uri)) == NULL)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (item->id[0] == '\0' || item->name[0] == '\0' || item->uri[0] == '\0' ||
        item->suite_id[0] == '\0' ||
        !item->discovered || strcmp(item->framework, "ctest") != 0 ||
        strcmp(item->kind, "test") != 0) return UMI_STATUS_INVALID_STATE;
    UmiTestPlatformDiscoverySnapshot discovery;
    char identity[sizeof(discovery.id)];
    int written = snprintf(identity, sizeof(identity), "discovery.%s", item->suite_id);
    if (written < 0 || (size_t)written >= sizeof(identity))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_test_platform_discovery_registry_find(discoveries, identity, &discovery);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(discovery.provider, "ctest-json-v1") != 0 || discovery.state != 1 ||
        strcmp(discovery.root_uri, item->uri) != 0) return UMI_STATUS_INVALID_STATE;
    UmiCtestJobRequest request = {0};
    memcpy(request.test_id, item->id, sizeof(item->id));
    memcpy(request.name, item->name, sizeof(item->name));
    memcpy(request.build_directory, item->uri, sizeof(item->uri));
    memcpy(request.configuration, discovery.configuration, sizeof(discovery.configuration));
    if (request.configuration[0] == '\0')
        memcpy(request.configuration, "Debug", sizeof("Debug"));
    request.enabled = item->enabled != 0;
    *outRequest = request;
    return UMI_STATUS_OK;
}
