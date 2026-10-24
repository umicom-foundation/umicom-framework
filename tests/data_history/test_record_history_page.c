/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Stable pages must cover each revision once and keep caller output on errors.
 * Fixtures use metadata-only history; payload integrity has separate coverage. */
#include "umicom/data/record_history_page.h"
#include "../../src/data/include/record_history_internal.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static int populate(UmiDataServer *server, uint64_t count)
{
    UmiRecordAmendment metadata = {.entity_id = "synthetic", .source = "fixture", .actor = "reviewer",
        .reason = "History exercise", .content_sha256 = "0000000000000000000000000000000000000000000000000000000000000000"};
    UmiRecordRevision row;
    for (uint64_t i = 0U; i < count; ++i) {
        metadata.recorded_at_nanoseconds = i + 1U;
        OK(umi_record_history_append(server, &metadata, i, &row));
    }
    return 0;
}
static int exercise(UmiDataServer *server, const char *name)
{
    UmiRecordHistoryPage page;
    memset(&page, 0xa5, sizeof page);
    unsigned char unchanged[sizeof page]; memcpy(unchanged, &page, sizeof page);
    if (strcmp(name, "empty") == 0) {
        CHECK(umi_record_history_page(server, "synthetic", 0U, 0U, 4U, &page) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&page, unchanged, sizeof page) == 0);
        CHECK(!umi_data_server_in_transaction(server));
        return 0;
    }
    CHECK(populate(server, 19U) == 0);
    if (strcmp(name, "pages") == 0) {
        OK(umi_record_history_page(server, "synthetic", 0U, 0U, 16U, &page));
        CHECK(page.head_revision == 19U && page.count == 16U && page.next_before_revision == 4U);
        for (size_t i = 0U; i < page.count; ++i) CHECK(page.rows[i].revision == 19U - i);
        /* Passing the output's own entity string must not lose its identity. */
        OK(umi_record_history_page(server, page.entity_id, page.head_revision, page.next_before_revision, 16U, &page));
        CHECK(page.count == 3U && page.next_before_revision == 0U);
        for (size_t i = 0U; i < page.count; ++i) CHECK(page.rows[i].revision == 3U - i);
        OK(umi_record_history_page(server, "synthetic", 19U, 1U, 1U, &page));
        CHECK(page.count == 0U && page.next_before_revision == 0U);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(umi_record_history_page(NULL, "synthetic", 0U, 0U, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, NULL, 0U, 0U, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "", 0U, 0U, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "bad\nkey", 0U, 0U, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "synthetic", 0U, 0U, 0U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "synthetic", 0U, 0U, 17U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "synthetic", 0U, 4U, 4U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_history_page(server, "synthetic", 19U, 20U, 4U, &page) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&page, unchanged, sizeof page) == 0);
    } else if (strcmp(name, "changed-head") == 0) {
        OK(umi_record_history_page(server, "synthetic", 0U, 0U, 4U, &page));
        UmiRecordRevision row; OK(umi_record_history_latest(server, "synthetic", &row));
        row.amendment.recorded_at_nanoseconds++;
        OK(umi_record_history_append(server, &row.amendment, 19U, &row));
        memcpy(unchanged, &page, sizeof page);
        CHECK(umi_record_history_page(server, "synthetic", page.head_revision, page.next_before_revision, 4U, &page) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&page, unchanged, sizeof page) == 0);
    } else if (strcmp(name, "nested") == 0) {
        OK(umi_data_server_begin(server));
        CHECK(umi_record_history_page(server, "synthetic", 0U, 0U, 4U, &page) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server));
        CHECK(memcmp(&page, unchanged, sizeof page) == 0);
        OK(umi_data_server_rollback(server));
    } else if (strcmp(name, "missing-row") == 0 || strcmp(name, "time-boundary") == 0) {
        char key[192]; OK(umi_record_history_key("synthetic", 15U, key, sizeof key));
        if (strcmp(name, "missing-row") == 0) OK(umi_data_server_delete(server, key));
        else {
            UmiRecordRevision row; OK(umi_record_history_load(server, "synthetic", 15U, &row));
            row.amendment.recorded_at_nanoseconds = 100U;
            char text[UMI_RECORD_REVISION_TEXT_CAPACITY]; OK(umi_record_revision_format(&row, text, sizeof text));
            OK(umi_data_server_set(server, key, text));
        }
        UmiStatus expected = strcmp(name, "missing-row") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_PARSE_ERROR;
        CHECK(umi_record_history_page(server, "synthetic", 19U, 16U, 4U, &page) == expected);
        CHECK(memcmp(&page, unchanged, sizeof page) == 0);
    } else return 2;
    CHECK(!umi_data_server_in_transaction(server));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (strcmp(argv[1], "memory") == 0) status = umi_data_server_create_memory(&server);
    else if (strcmp(argv[1], "sqlite") == 0) status = umi_data_server_create_sqlite(":memory:", &server);
    else return 2;
    if (status != UMI_STATUS_OK) return 1;
    int result = exercise(server, argv[2]);
    umi_data_server_destroy(server);
    return result;
}
