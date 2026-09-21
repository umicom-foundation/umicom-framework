/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/json_test_support.c
 * PURPOSE: Retain and compare all three registry states during failed imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "json_test_support.h"
void JsonTestCreate(JsonTestState *state)
{
    memset(state, 0, sizeof(*state));
    REQUIRE(umi_test_platform_item_registry_create(&state->items) == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_suite_registry_create(&state->suites) == UMI_STATUS_OK);
    REQUIRE(umi_test_platform_discovery_registry_create(&state->discoveries) == UMI_STATUS_OK);
    memcpy(state->options.project_id, "notes", sizeof("notes"));
    memcpy(state->options.suite_id, "notes.ctest", sizeof("notes.ctest"));
    memcpy(state->options.configuration, "Debug", sizeof("Debug"));
    memcpy(state->options.build_directory, "/configured notes", sizeof("/configured notes"));
}
void JsonTestDestroy(JsonTestState *state)
{
    umi_test_platform_item_registry_destroy(state->items);
    umi_test_platform_suite_registry_destroy(state->suites);
    umi_test_platform_discovery_registry_destroy(state->discoveries);
    memset(state, 0, sizeof(*state));
}
UmiStatus JsonTestImport(JsonTestState *state, const char *json, UmiTestPlatformCtestImportSummary *summary)
{
    return UmiTestPlatformCtestParseJsonSized(json, strlen(json), &state->options,
        state->items, state->suites, state->discoveries, summary);
}
void JsonTestSeed(JsonTestState *state)
{
    UmiTestPlatformCtestImportSummary summary;
    const char *seed = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[{\"name\":\"old\"}]}";
    REQUIRE(JsonTestImport(state, seed, &summary) == UMI_STATUS_OK);
    memcpy(state->options.project_id, "bank", sizeof("bank"));
    memcpy(state->options.suite_id, "bank.ctest", sizeof("bank.ctest"));
    REQUIRE(JsonTestImport(state, seed, &summary) == UMI_STATUS_OK);
    memcpy(state->options.project_id, "notes", sizeof("notes"));
    memcpy(state->options.suite_id, "notes.ctest", sizeof("notes.ctest"));
}
static uint64_t Mix(uint64_t hash, const void *data, size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0U; i < length; ++i) { hash ^= bytes[i]; hash *= UINT64_C(1099511628211); }
    return hash;
}
uint64_t JsonTestFingerprint(const JsonTestState *state)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    uint64_t revisions[3] = {umi_test_platform_item_registry_revision(state->items),
        umi_test_platform_suite_registry_revision(state->suites),
        umi_test_platform_discovery_registry_revision(state->discoveries)};
    size_t counts[3] = {umi_test_platform_item_registry_count(state->items),
        umi_test_platform_suite_registry_count(state->suites),
        umi_test_platform_discovery_registry_count(state->discoveries)};
    hash = Mix(hash, revisions, sizeof(revisions)); hash = Mix(hash, counts, sizeof(counts));
    for (size_t i = 0U; i < counts[0]; ++i) {
        UmiTestPlatformItemSnapshot item;
        REQUIRE(umi_test_platform_item_registry_at(state->items, i, &item) == UMI_STATUS_OK);
        hash = Mix(hash, &item, sizeof(item));
    }
    for (size_t i = 0U; i < counts[1]; ++i) {
        UmiTestPlatformSuiteSnapshot item;
        REQUIRE(umi_test_platform_suite_registry_at(state->suites, i, &item) == UMI_STATUS_OK);
        hash = Mix(hash, &item, sizeof(item));
    }
    for (size_t i = 0U; i < counts[2]; ++i) {
        UmiTestPlatformDiscoverySnapshot item;
        REQUIRE(umi_test_platform_discovery_registry_at(state->discoveries, i, &item) == UMI_STATUS_OK);
        hash = Mix(hash, &item, sizeof(item));
    }
    return hash;
}
void JsonTestItem(const JsonTestState *state, const char *name, UmiTestPlatformItemSnapshot *out)
{
    for (size_t i = 0U; i < umi_test_platform_item_registry_count(state->items); ++i) {
        REQUIRE(umi_test_platform_item_registry_at(state->items, i, out) == UMI_STATUS_OK);
        if (strcmp(out->suite_id, state->options.suite_id) == 0 && strcmp(out->name, name) == 0) return;
    }
    fprintf(stderr, "Expected test name absent: %s\n", name); REQUIRE(0);
}
