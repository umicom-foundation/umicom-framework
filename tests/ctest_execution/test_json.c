/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_json.c
 * PURPOSE: Verify strict protocol metadata and failure-atomic refresh publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "json_test_support.h"
typedef struct JsonVector {
    const char *name; const char *json; size_t length; UmiStatus expected;
    size_t count; uint32_t timeout; size_t disabled;
} JsonVector;
#include "json_vectors.inc"
static void Vector(const JsonVector *v)
{
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    UmiTestPlatformItemSnapshot bank, after, item;
    JsonTestCreate(&s); JsonTestSeed(&s);
    REQUIRE(umi_test_platform_item_registry_find(s.items, "bank.ctest.old", &bank) == UMI_STATUS_OK);
    uint64_t before = JsonTestFingerprint(&s);
    memset(&summary, 0xff, sizeof(summary));
    UmiStatus status = UmiTestPlatformCtestParseJsonSized(v->json, v->length, &s.options,
        s.items, s.suites, s.discoveries, &summary);
    if (status != v->expected) fprintf(stderr, "%s: expected %d got %d\n", v->name, (int)v->expected, (int)status);
    REQUIRE(status == v->expected);
    if (status != UMI_STATUS_OK) {
        REQUIRE(JsonTestFingerprint(&s) == before);
        REQUIRE(summary.discovered_count == 0U && summary.disabled_count == 0U &&
            summary.labelled_count == 0U && summary.timed_count == 0U);
    } else {
        REQUIRE(summary.discovered_count == v->count && summary.disabled_count == v->disabled);
        REQUIRE(umi_test_platform_item_registry_count(s.items) == v->count + 3U);
        REQUIRE(umi_test_platform_item_registry_find(s.items, "notes.ctest.old", &item) == UMI_STATUS_NOT_FOUND);
        REQUIRE(umi_test_platform_item_registry_find(s.items, "bank.ctest.old", &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&bank, &after, sizeof(bank)) == 0);
        if (v->count != 0U) {
            REQUIRE(umi_test_platform_item_registry_at(s.items, 3U, &item) == UMI_STATUS_OK);
            REQUIRE(item.timeout_ms == v->timeout);
            REQUIRE(item.enabled == (v->disabled == 0U));
            REQUIRE(strcmp(item.uri, s.options.build_directory) == 0);
            if (strcmp(v->name, "metadata") == 0) {
                REQUIRE(strcmp(item.name, "notes.save") == 0);
                REQUIRE(strcmp(item.source_uri, "/source/CMakeLists.txt") == 0 && item.source_line == 27U);
                REQUIRE(strcmp(item.working_directory, "/working notes") == 0);
                REQUIRE(strcmp(item.command, "cmake -E true") == 0);
                REQUIRE(strcmp(item.labels, "learning;notes") == 0);
                REQUIRE(summary.labelled_count == 1U && summary.timed_count == 1U);
            }
            if (strcmp(v->name, "unicode") == 0) REQUIRE(strcmp(item.name, "notes.\346\227\245\346\234\254.\360\237\230\200") == 0);
            if (strcmp(v->name, "name-255") == 0) {
                char id[128]; memcpy(id, item.id, sizeof(id));
                REQUIRE(strlen(item.name) == 255U && strlen(item.id) < 128U);
                REQUIRE(UmiTestPlatformCtestParseJsonSized(v->json, v->length, &s.options, s.items, s.suites, s.discoveries, &summary) == UMI_STATUS_OK);
                REQUIRE(umi_test_platform_item_registry_find(s.items, id, &item) == UMI_STATUS_OK);
            }
        }
    }
    printf("%s: status=%d discovered=%zu retained=%zu\n", v->name, (int)status, summary.discovered_count,
        umi_test_platform_item_registry_count(s.items));
    JsonTestDestroy(&s);
}
static void Refresh(void)
{
    JsonTestState s; UmiTestPlatformCtestImportSummary summary; UmiTestPlatformItemSnapshot item;
    UmiTestPlatformDiscoverySnapshot discovery; UmiTestPlatformSuiteSnapshot suite;
    JsonTestCreate(&s); JsonTestSeed(&s);
    const char *json = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[{\"name\":\"new\"}]}";
    memcpy(s.options.build_directory, "/new build root", sizeof("/new build root"));
    memcpy(s.options.configuration, "Release", sizeof("Release"));
    REQUIRE(JsonTestImport(&s, json, &summary) == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_item_registry_find(s.items, "notes.ctest.new", &item) == UMI_STATUS_OK);
    REQUIRE(strcmp(item.uri, "/new build root") == 0);
    REQUIRE(umi_test_platform_item_registry_find(s.items, "notes.ctest.old", &item) == UMI_STATUS_NOT_FOUND);
    REQUIRE(umi_test_platform_discovery_registry_find(s.discoveries, "discovery.notes.ctest", &discovery) == UMI_STATUS_OK);
    REQUIRE(strcmp(discovery.configuration, "Release") == 0 && strcmp(discovery.root_uri, "/new build root") == 0);
    REQUIRE(umi_test_platform_suite_registry_find(s.suites, "notes.ctest", &suite) == UMI_STATUS_OK && suite.test_count == 1U);
    uint64_t before = JsonTestFingerprint(&s);
    memcpy(s.options.project_id, "somebody-else", sizeof("somebody-else"));
    REQUIRE(JsonTestImport(&s, json, &summary) == UMI_STATUS_INVALID_STATE);
    REQUIRE(JsonTestFingerprint(&s) == before);
    JsonTestDestroy(&s);
}
static void Compatibility(void)
{
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    JsonTestCreate(&s);
    const char *json = "{\"tests\":[{\"name\":\"notes\"}]}";
    REQUIRE(umi_test_platform_ctest_parse_json(json, &s.options, s.items, s.suites, s.discoveries, &summary) == UMI_STATUS_OK);
    uint64_t before = JsonTestFingerprint(&s);
    REQUIRE(JsonTestImport(&s, json, &summary) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(JsonTestFingerprint(&s) == before);
    const char *historical = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1},\"tests\":[{\"name\":\"framework.alpha\",\"command\":[\"C:/build/alpha.exe\",\"--verbose\"],\"properties\":[{\"name\":\"LABELS\",\"value\":[\"unit\",\"fast\"]},{\"name\":\"TIMEOUT\",\"value\":30.0},{\"name\":\"WORKING_DIRECTORY\",\"value\":\"C:/build\"}]},{\"name\":\"framework.beta\",\"command\":[\"C:/build/beta.exe\"],\"properties\":[{\"name\":\"DISABLED\",\"value\":true}]}]}";
    REQUIRE(umi_test_platform_ctest_parse_json(historical, &s.options, s.items, s.suites, s.discoveries, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.discovered_count == 2U && summary.disabled_count == 1U && summary.timed_count == 1U && summary.labelled_count == 1U);
    UmiTestPlatformItemSnapshot item;
    JsonTestItem(&s, "framework.alpha", &item);
    REQUIRE(strcmp(item.command, "C:/build/alpha.exe --verbose") == 0 && item.timeout_ms == 30000U);
    REQUIRE(strcmp(item.labels, "unit;fast") == 0 && strcmp(item.working_directory, "C:/build") == 0);
    before = JsonTestFingerprint(&s);
    REQUIRE(JsonTestImport(&s, historical, &summary) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(JsonTestFingerprint(&s) == before);
    JsonTestDestroy(&s);
}
static void Collision(void)
{
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    JsonTestCreate(&s);
    memcpy(s.options.suite_id, "a", sizeof("a"));
    const char *first = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[{\"name\":\"b.c\"}]}";
    const char *second = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[{\"name\":\"c\"}]}";
    REQUIRE(JsonTestImport(&s, first, &summary) == UMI_STATUS_OK);
    uint64_t before = JsonTestFingerprint(&s);
    memcpy(s.options.suite_id, "a.b", sizeof("a.b"));
    REQUIRE(JsonTestImport(&s, second, &summary) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(JsonTestFingerprint(&s) == before);
    JsonTestDestroy(&s);
}
static void Truncations(void)
{
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    const char *json = vectors[2].json;
    JsonTestCreate(&s); JsonTestSeed(&s);
    uint64_t before = JsonTestFingerprint(&s);
    for (size_t n = 0U; n < strlen(json); ++n) {
        REQUIRE(UmiTestPlatformCtestParseJsonSized(json, n, &s.options, s.items, s.suites, s.discoveries, &summary) != UMI_STATUS_OK);
        REQUIRE(JsonTestFingerprint(&s) == before);
    }
    printf("Rejected %zu incomplete prefixes; all registry records and revisions preserved.\n", strlen(json));
    JsonTestDestroy(&s);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    if (strcmp(argv[1], "refresh") == 0) { Refresh(); return 0; }
    if (strcmp(argv[1], "compatibility") == 0) { Compatibility(); return 0; }
    if (strcmp(argv[1], "collision") == 0) { Collision(); return 0; }
    if (strcmp(argv[1], "truncations") == 0) { Truncations(); return 0; }
    for (size_t i = 0U; i < sizeof(vectors)/sizeof(vectors[0]); ++i)
        if (strcmp(argv[1], vectors[i].name) == 0) { Vector(&vectors[i]); return 0; }
    return 2;
}
