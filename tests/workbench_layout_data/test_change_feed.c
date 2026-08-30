/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_change_feed.c
 *
 * PURPOSE:
 *   Verify ordered layout change records, sequence-key ordering, filtering and durable audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiStatus change_for_layout(
    const void *record_value,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutChange *change =
        (const UmiWorkbenchLayoutChange *)record_value;
    const char *layout_id = (const char *)context;
    if (change == NULL || layout_id == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = strcmp(change->layout_id, layout_id) == 0;
    return UMI_STATUS_OK;
}

static int test_change_codec(void)
{
    UmiWorkbenchLayoutChange source = test_create_change(
        "change-9", "layout.development",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        9U, 8U, 9U, 12345U);
    UmiWorkbenchLayoutChange decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_change_feed_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        "change kind round trip");
    TEST_REQUIRE(decoded.sequence == 9U,
                 "change sequence round trip");
    TEST_REQUIRE(decoded.base_revision == 8U,
                 "base revision round trip");
    TEST_REQUIRE(decoded.content_hash == 12345U,
                 "content hash round trip");
    return 0;
}

static int test_ordered_feed(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChangeFeedRepository repository;
    UmiWorkbenchLayoutChange changes[4];
    UmiWorkbenchLayoutChange results[4];
    size_t count = 0U;
    size_t total = 0U;
    size_t index;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_change_feed_repository_init(
        &repository, server));
    changes[0] = test_create_change("change-30", "layout.a",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED, 30U, 2U, 3U, 300U);
    changes[1] = test_create_change("change-10", "layout.a",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED, 10U, 0U, 1U, 100U);
    changes[2] = test_create_change("change-20", "layout.b",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED, 20U, 0U, 1U, 200U);
    changes[3] = test_create_change("change-25", "layout.a",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED, 25U, 1U, 2U, 250U);
    for (index = 0U; index < 4U; ++index) {
        TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
            &repository, &changes[index]));
    }
    TEST_STATUS_OK(umi_workbench_layout_change_feed_list(
        &repository, NULL, change_for_layout, "layout.a",
        results, 4U, &count, &total));
    TEST_REQUIRE(count == 3U && total == 3U,
                 "layout changes filtered");
    TEST_REQUIRE(results[0].sequence == 10U,
                 "first sequence ordered");
    TEST_REQUIRE(results[1].sequence == 25U,
                 "middle sequence ordered");
    TEST_REQUIRE(results[2].sequence == 30U,
                 "last sequence ordered");
    umi_data_server_destroy(server);
    return 0;
}

static int test_change_load_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChangeFeedRepository repository;
    UmiWorkbenchLayoutChange source = test_create_change(
        "change-load", "layout.a",
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_LOCKED,
        44U, 3U, 4U, 444U);
    UmiWorkbenchLayoutChange loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_change_feed_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_save(
        &repository, &source));
    TEST_STATUS_OK(umi_workbench_layout_change_feed_load(
        &repository, NULL, "change-load", 44U, &loaded));
    TEST_REQUIRE(loaded.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_LOCKED,
        "change loaded");
    TEST_STATUS_OK(umi_workbench_layout_change_feed_delete(
        &repository, NULL, "change-load", 44U));
    TEST_REQUIRE(umi_workbench_layout_change_feed_load(
        &repository, NULL, "change-load", 44U, &loaded) ==
        UMI_STATUS_NOT_FOUND, "change deleted");
    umi_data_server_destroy(server);
    return 0;
}

static int test_invalid_change(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChangeFeedRepository repository;
    UmiWorkbenchLayoutChange change;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_change_feed_repository_init(
        &repository, server));
    (void)memset(&change, 0, sizeof(change));
    change.structure_size = sizeof(change);
    TEST_REQUIRE(umi_workbench_layout_change_feed_save(
        &repository, &change) == UMI_STATUS_INVALID_ARGUMENT,
        "missing identity rejected");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_change_codec() == 0, "change codec");
    TEST_REQUIRE(test_ordered_feed() == 0, "ordered feed");
    TEST_REQUIRE(test_change_load_delete() == 0,
                 "change load delete");
    TEST_REQUIRE(test_invalid_change() == 0, "invalid change");
    return 0;
}
