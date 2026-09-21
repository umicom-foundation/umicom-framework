/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_json_native.c
 * PURPOSE: Compare every record from real CTest with its deterministic source catalogue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "json_test_support.h"
#include "umicom/testing/ctest_adapter.h"
static void Name(size_t index, char *out, size_t capacity)
{
    if (index == 1U) {
        REQUIRE(capacity > 206U); memcpy(out, "notes.", 6U); memset(out + 6U, 'x', 200U); out[206] = '\0';
    } else if (index == 2U) {
        REQUIRE(capacity > strlen("notes.literal (Disabled)")); strcpy(out, "notes.literal (Disabled)");
    } else if (index == 3U) {
        const char name[] = "notes.\303\251.\346\227\245\346\234\254";
        REQUIRE(capacity >= sizeof(name)); memcpy(out, name, sizeof(name));
    } else {
        int n = snprintf(out, capacity, "notes.case%zu", index);
        REQUIRE(n >= 0 && (size_t)n < capacity);
    }
}
int main(int argc, char **argv)
{
    REQUIRE(argc >= 3);
    const char *root = argv[1], *scenario = argv[2];
    JsonTestState s; UmiTestPlatformCtestDiscoveryOptions control = {0};
    UmiTestPlatformCtestImportSummary summary; UmiStatus expected = UMI_STATUS_OK;
    UmiCancellationToken *token = NULL;
    char diagnostic[2048], marker[1200], working[1200];
    size_t count = strcmp(scenario, "empty") == 0 ? 0U : 10000U;
    JsonTestCreate(&s); JsonTestSeed(&s);
    REQUIRE(strlen(root) < sizeof(s.options.build_directory));
    strcpy(s.options.build_directory, root);
    REQUIRE(snprintf(marker, sizeof(marker), "%s/unexpected-test-execution.marker", root) > 0);
    REQUIRE(snprintf(working, sizeof(working), "%s/working notes", root) > 0);
    FILE *present = fopen(marker, "rb");
    REQUIRE(present == NULL);
    if (strcmp(scenario, "capture-limit") == 0) { control.maximum_output_bytes = 65536U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(scenario, "record-limit") == 0) { control.maximum_tests = 9999U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(scenario, "missing-root") == 0) expected = UMI_STATUS_NOT_FOUND;
    else if (strcmp(scenario, "cancel-before") == 0) {
        REQUIRE(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token); control.cancellation = token; expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(scenario, "timeout") == 0) { control.timeout_ms = 10U; expected = UMI_STATUS_TIMEOUT; }
    else if (strcmp(scenario, "invalid-limit") == 0) { control.maximum_output_bytes = UMI_TEST_PLATFORM_CTEST_MAX_BYTES + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    uint64_t before = JsonTestFingerprint(&s);
    UmiStatus status;
    if (strcmp(scenario, "legacy-entry") == 0)
        status = umi_test_platform_ctest_discover(&s.options, s.items, s.suites, s.discoveries, &summary, diagnostic, sizeof(diagnostic));
    else
        status = UmiTestPlatformCtestDiscoverConfigured(&s.options, &control, s.items, s.suites, s.discoveries,
            &summary, diagnostic, sizeof(diagnostic));
    if (status != expected) fprintf(stderr, "Expected %d got %d: %s\n", (int)expected, (int)status, diagnostic);
    REQUIRE(status == expected);
    if (status != UMI_STATUS_OK) {
        REQUIRE(JsonTestFingerprint(&s) == before && summary.discovered_count == 0U);
        REQUIRE(diagnostic[0] != '\0');
        printf("%s: status=%d; all registry records and revisions preserved. %s\n", scenario, (int)status, diagnostic);
    } else {
        REQUIRE(summary.discovered_count == count && summary.disabled_count == count / 100U);
        REQUIRE(summary.labelled_count == count && summary.timed_count == count);
        REQUIRE(umi_test_platform_item_registry_count(s.items) == count + 3U);
        REQUIRE(diagnostic[0] == '\0');
        FILE *dump = argc == 4 ? fopen(argv[3], "wb") : NULL;
        if (argc == 4) REQUIRE(dump != NULL);
        for (size_t i = 0U; i < count; ++i) {
            UmiTestPlatformItemSnapshot item, found; char expectedName[256];
            REQUIRE(umi_test_platform_item_registry_at(s.items, i + 3U, &item) == UMI_STATUS_OK);
            Name(i, expectedName, sizeof(expectedName));
            REQUIRE(strcmp(item.name, expectedName) == 0);
            REQUIRE(item.enabled == (i % 100U != 0U));
            REQUIRE(item.timeout_ms == 1250U && item.discovered == 1);
            REQUIRE(strcmp(item.labels, "learning;notes") == 0);
            REQUIRE(strcmp(item.uri, root) == 0 && strcmp(item.working_directory, working) == 0);
            REQUIRE(strstr(item.source_uri, "json_fixture/CMakeLists.txt") != NULL && item.source_line != 0U);
            REQUIRE(umi_test_platform_item_registry_find(s.items, item.id, &found) == UMI_STATUS_OK);
            REQUIRE(strcmp(found.name, item.name) == 0);
            if (dump != NULL) REQUIRE(fprintf(dump, "%s\t%d\t%u\t%s\t%u\t%s\t%s\n", item.name, item.enabled,
                item.timeout_ms, item.source_uri, item.source_line, item.labels, item.working_directory) > 0);
        }
        if (dump != NULL) REQUIRE(fclose(dump) == 0);
        printf("%s: discovered=%zu disabled=%zu labelled=%zu timed=%zu; every name, ID lookup, source, timeout and working directory checked.\n",
            scenario, summary.discovered_count, summary.disabled_count, summary.labelled_count, summary.timed_count);
        if (strcmp(scenario, "execution-bridge") == 0) {
            UmiTestPlatformItemSnapshot item; JsonTestItem(&s, "notes.case9999", &item);
            UmiCtestRunOptions execution = {0}; UmiTestResult result;
            execution.configuration = s.options.configuration; execution.test_id = item.id;
            REQUIRE(UmiCtestRunConfigured(item.uri, item.name, &execution, &result) == UMI_STATUS_OK);
            REQUIRE(result.state == UMI_TEST_STATE_PASSED);
            present = fopen(marker, "rb"); REQUIRE(present != NULL); REQUIRE(fclose(present) == 0);
            REQUIRE(remove(marker) == 0);
        }
    }
    present = fopen(marker, "rb"); REQUIRE(present == NULL);
    umi_cancellation_token_destroy(token); JsonTestDestroy(&s);
    return 0;
}
