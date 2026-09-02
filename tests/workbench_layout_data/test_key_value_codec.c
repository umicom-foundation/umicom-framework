/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_key_value_codec.c
 *
 * PURPOSE:
 *   Verify deterministic Data Server keys and escaped bounded value encoding for every persisted layout record family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Exercise test layout keys and return a clear result when the behaviour no longer matches
 * its contract.
 */
static int test_layout_keys(void)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char prefix[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiWorkbenchLayoutDataKeyParts parts;

    TEST_STATUS_OK(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        "layout.development", NULL, 0U, 0U, key, sizeof(key)));
    TEST_REQUIRE(strcmp(key,
        "workbench-layout/layout-manifest/layout.development") == 0,
        "layout manifest key");
    TEST_STATUS_OK(umi_workbench_layout_data_key_parse(key, &parts));
    TEST_REQUIRE(parts.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        "manifest kind parsed");
    TEST_REQUIRE(strcmp(parts.aggregate_id, "layout.development") == 0,
        "aggregate parsed");

    TEST_STATUS_OK(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
        "layout.development", NULL, 0U, 7U, key, sizeof(key)));
    TEST_REQUIRE(strcmp(key,
        "workbench-layout/layout-chunk/layout.development/7") == 0,
        "layout chunk key");
    TEST_STATUS_OK(umi_workbench_layout_data_key_parse(key, &parts));
    TEST_REQUIRE(parts.chunk_index == 7U, "chunk index parsed");

    TEST_STATUS_OK(umi_workbench_layout_data_key_prefix(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
        "layout.development", prefix, sizeof(prefix)));
    TEST_REQUIRE(umi_workbench_layout_data_key_matches_prefix(key, prefix),
        "chunk key matches prefix");
    TEST_REQUIRE(umi_workbench_layout_data_key_is_owned(key),
        "key belongs to Framework namespace");
    TEST_REQUIRE(!umi_workbench_layout_data_key_is_owned("other/key"),
        "foreign key rejected");
    return 0;
}

/*
 * Exercise test sequenced keys and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_sequenced_keys(void)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiWorkbenchLayoutDataKeyParts parts;
    TEST_STATUS_OK(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION,
        "layout.development", "revision-42", 42U, 0U,
        key, sizeof(key)));
    TEST_REQUIRE(strcmp(key,
        "workbench-layout/revision/00000000000000000042/revision-42") == 0,
        "revision key sorted by sequence");
    TEST_STATUS_OK(umi_workbench_layout_data_key_parse(key, &parts));
    TEST_REQUIRE(parts.sequence == 42U, "sequence parsed");
    TEST_REQUIRE(strcmp(parts.record_id, "revision-42") == 0,
        "record identity parsed");

    TEST_REQUIRE(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT,
        "bad/id", "grant", 0U, 0U, key, sizeof(key)) ==
        UMI_STATUS_INVALID_ARGUMENT, "slash rejected in segment");
    TEST_REQUIRE(umi_workbench_layout_data_key_parse(
        "workbench-layout/unknown/item", &parts) ==
        UMI_STATUS_PARSE_ERROR, "unknown segment rejected");
    return 0;
}

/*
 * Copy test field into module-owned storage so callers keep ownership of their input
 * values.
 */
static int test_field_set(void)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    uint64_t revision = 0U;
    uint32_t flags = 0U;
    bool enabled = false;
    umi_workbench_layout_data_field_set_init(&fields);
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &fields, "name", "Development\nWorkbench"));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put_u64(
        &fields, "revision", 184467U));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put_u32(
        &fields, "flags", 17U));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put_bool(
        &fields, "enabled", true));
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_field_set_get(
        &fields, "name"), "Development\nWorkbench") == 0,
        "field text available");
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_get_u64(
        &fields, "revision", &revision));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_get_u32(
        &fields, "flags", &flags));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_get_bool(
        &fields, "enabled", &enabled));
    TEST_REQUIRE(revision == 184467U, "u64 round trip");
    TEST_REQUIRE(flags == 17U, "u32 round trip");
    TEST_REQUIRE(enabled, "bool round trip");
    TEST_REQUIRE(umi_workbench_layout_data_field_set_get_u64(
        &fields, "missing", &revision) == UMI_STATUS_NOT_FOUND,
        "missing field reported");
    return 0;
}

/*
 * Exercise test value round trip and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_value_round_trip(void)
{
    UmiWorkbenchLayoutDataFieldSet source;
    UmiWorkbenchLayoutDataFieldSet decoded;
    char encoded[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    umi_workbench_layout_data_field_set_init(&source);
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &source, "plain", "value"));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &source, "escaped", "line one\nline two=ready\\done"));
    TEST_STATUS_OK(umi_workbench_layout_data_value_encode(
        &source, encoded, sizeof(encoded), &required));
    TEST_REQUIRE(required == strlen(encoded) + 1U,
                 "required includes terminator");
    TEST_STATUS_OK(umi_workbench_layout_data_value_decode(
        encoded, &decoded));
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_field_set_get(
        &decoded, "plain"), "value") == 0, "plain round trip");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_field_set_get(
        &decoded, "escaped"), "line one\nline two=ready\\done") == 0,
        "escaped round trip");
    TEST_REQUIRE(umi_workbench_layout_data_value_decode(
        "invalid-line-without-separator", &decoded) ==
        UMI_STATUS_PARSE_ERROR, "malformed line rejected");
    return 0;
}

/*
 * Exercise test escape capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_escape_capacity(void)
{
    char escaped[64];
    char restored[64];
    char small[4];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_data_value_escape(
        "a=b\\c\nd", escaped, sizeof(escaped), &required));
    TEST_REQUIRE(required == strlen(escaped) + 1U,
                 "escape required size");
    TEST_STATUS_OK(umi_workbench_layout_data_value_unescape(
        escaped, restored, sizeof(restored), &required));
    TEST_REQUIRE(strcmp(restored, "a=b\\c\nd") == 0,
                 "escape restored");
    TEST_REQUIRE(umi_workbench_layout_data_value_escape(
        "long", small, sizeof(small), &required) ==
        UMI_STATUS_CAPACITY_EXCEEDED, "escape capacity guarded");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_layout_keys() == 0, "layout keys");
    TEST_REQUIRE(test_sequenced_keys() == 0, "sequenced keys");
    TEST_REQUIRE(test_field_set() == 0, "field set");
    TEST_REQUIRE(test_value_round_trip() == 0, "value codec");
    TEST_REQUIRE(test_escape_capacity() == 0, "escape capacity");
    return 0;
}
