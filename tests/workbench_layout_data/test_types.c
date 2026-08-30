/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_types.c
 *
 * PURPOSE:
 *   Verify stable text, hashing, time and state-description contracts used by layout persistence records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_record_kind_text(void)
{
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST),
        "layout-manifest") == 0, "layout manifest text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK),
        "layout-chunk") == 0, "layout chunk text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST),
        "session-manifest") == 0, "session manifest text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK),
        "session-chunk") == 0, "session chunk text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION),
        "revision") == 0, "revision text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT),
        "grant") == 0, "grant text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE),
        "lease") == 0, "lease text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE),
        "change") == 0, "change text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX),
        "outbox") == 0, "outbox text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT),
        "conflict") == 0, "conflict text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR),
        "sync-cursor") == 0, "cursor text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION),
        "offline-operation") == 0, "offline text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE),
        "presence") == 0, "presence text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION),
        "migration") == 0, "migration text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_record_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA),
        "metadata") == 0, "metadata text");
    return 0;
}

static int test_state_text(void)
{
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_change_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED), "updated") == 0,
        "change text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_permission_text(
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_ADMINISTER), "administer") == 0,
        "permission text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_lease_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD), "held") == 0,
        "lease state text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_outbox_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DEAD_LETTER), "dead-letter") == 0,
        "outbox state text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_conflict_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_INTEGRITY), "integrity") == 0,
        "conflict kind text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_conflict_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED),
        "resolved-merged") == 0, "conflict state text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_sync_action_text(
        UMI_WORKBENCH_LAYOUT_DATA_SYNC_UPDATE_REMOTE),
        "update-remote") == 0, "sync action text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_operation_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT),
        "retry-wait") == 0, "operation text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_health_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_HEALTH_DEGRADED), "degraded") == 0,
        "health text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_controller_state_text(
        UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED), "quiesced") == 0,
        "controller text");
    return 0;
}

static int test_hashing(void)
{
    const char payload[] = "layout-state";
    const uint64_t first = umi_workbench_layout_data_hash_bytes(
        payload, sizeof(payload) - 1U);
    const uint64_t second = umi_workbench_layout_data_hash_text(payload);
    const uint64_t different = umi_workbench_layout_data_hash_text(
        "layout-state-changed");
    TEST_REQUIRE(first != 0U, "hash must be non-zero");
    TEST_REQUIRE(first == second, "byte and text hash must agree");
    TEST_REQUIRE(first != different, "different payload must differ");
    TEST_REQUIRE(umi_workbench_layout_data_hash_text("") != 0U,
                 "empty text has stable hash");
    return 0;
}

static int test_text_helpers(void)
{
    char destination[16];
    char small[4];
    TEST_STATUS_OK(umi_workbench_layout_data_copy_text(
        destination, sizeof(destination), "layout", false));
    TEST_REQUIRE(strcmp(destination, "layout") == 0, "copied text");
    TEST_REQUIRE(umi_workbench_layout_data_copy_text(
        small, sizeof(small), "layout", false) ==
        UMI_STATUS_CAPACITY_EXCEEDED, "bounded copy");
    TEST_REQUIRE(umi_workbench_layout_data_copy_text(
        destination, sizeof(destination), "", false) ==
        UMI_STATUS_INVALID_ARGUMENT, "empty denied");
    TEST_STATUS_OK(umi_workbench_layout_data_copy_text(
        destination, sizeof(destination), "", true));
    TEST_REQUIRE(destination[0] == '\0', "empty allowed");
    TEST_REQUIRE(umi_workbench_layout_data_text_equal("a", "a"),
                 "equal text");
    TEST_REQUIRE(!umi_workbench_layout_data_text_equal("a", "b"),
                 "different text");
    TEST_REQUIRE(umi_workbench_layout_data_text_has_prefix(
        "workbench-layout/revision", "workbench-layout/"),
        "prefix match");
    TEST_REQUIRE(!umi_workbench_layout_data_text_has_prefix(
        "other", "workbench-layout/"), "prefix mismatch");
    return 0;
}

static int test_expiry(void)
{
    TEST_REQUIRE(!umi_workbench_layout_data_time_expired(0U, 500U),
                 "zero expiry means unlimited");
    TEST_REQUIRE(!umi_workbench_layout_data_time_expired(1000U, 999U),
                 "future expiry is active");
    TEST_REQUIRE(umi_workbench_layout_data_time_expired(1000U, 1000U),
                 "deadline is expired");
    TEST_REQUIRE(umi_workbench_layout_data_time_expired(1000U, 1001U),
                 "past expiry is expired");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_record_kind_text() == 0, "record kinds");
    TEST_REQUIRE(test_state_text() == 0, "state text");
    TEST_REQUIRE(test_hashing() == 0, "hashing");
    TEST_REQUIRE(test_text_helpers() == 0, "text helpers");
    TEST_REQUIRE(test_expiry() == 0, "expiry");
    return 0;
}
