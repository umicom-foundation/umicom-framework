/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_lineage.c
 *
 * PURPOSE:
 *   Reopen, provider failure and legacy-text regressions use real public APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reopen, provider failure and legacy-text regressions use real public APIs. */
#include "umicom/data/lineage.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static UmiStatus unavailable_get(void *instance, const char *key, char *out, size_t capacity)
{
    (void)instance; (void)key; (void)out; (void)capacity;
    return UMI_STATUS_NOT_FOUND;
}
static UmiStatus failed_set(void *instance, const char *key, const char *value)
{
    (void)instance; (void)key; (void)value;
    return UMI_STATUS_IO_ERROR;
}
static int exercise(UmiDataServer *server, const char *name)
{
    UmiStore store;
    UmiLineageStore first, second;
    UmiLineageRecord record = {.entity_id = "synthetic-document", .operation = "save",
        .source = "test", .actor = "fictional-editor", .correlation_id = UINT64_MAX,
        .occurred_at_nanoseconds = UINT64_MAX};
    UmiLineageRecord loaded;
    OK(umi_store_from_data_server(server, &store));
    OK(umi_lineage_store_init(&first, &store));
    if (strcmp(name, "reopen") == 0) {
        OK(umi_lineage_append(&first, &record));
        CHECK(record.sequence == 1U);
        OK(umi_lineage_store_init(&second, &store));
        strcpy(record.operation, "amend");
        OK(umi_lineage_append(&second, &record));
        CHECK(record.sequence == 2U);
        OK(umi_lineage_load(&first, 1U, &loaded));
        CHECK(strcmp(loaded.operation, "save") == 0);
        OK(umi_lineage_append(&first, &record));
        CHECK(record.sequence == 3U);
    } else if (strcmp(name, "empty-fields") == 0) {
        record.operation[0] = '\0'; record.source[0] = '\0'; record.actor[0] = '\0';
        OK(umi_lineage_append(&first, &record));
        OK(umi_lineage_load(&first, 1U, &loaded));
        CHECK(!loaded.operation[0] && !loaded.source[0] && !loaded.actor[0]);
        CHECK(loaded.correlation_id == UINT64_MAX && loaded.occurred_at_nanoseconds == UINT64_MAX);
    } else if (strcmp(name, "failed-save") == 0) {
        UmiStore failing = {.structure_size = (uint32_t)sizeof(UmiStore),
            .set = failed_set, .get = unavailable_get};
        OK(umi_lineage_store_init(&second, &failing));
        record.sequence = 99U;
        CHECK(umi_lineage_append(&second, &record) == UMI_STATUS_IO_ERROR);
        CHECK(record.sequence == 99U && second.next_sequence == 1U);
    } else if (strcmp(name, "invalid-fields") == 0) {
        strcpy(record.actor, "person\tinjected");
        CHECK(umi_lineage_append(&first, &record) == UMI_STATUS_INVALID_ARGUMENT);
        memset(record.actor, 'x', sizeof record.actor);
        CHECK(umi_lineage_append(&first, &record) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(record.sequence == 0U && first.next_sequence == 1U);
        size_t count = 99U;
        OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 0U);
    } else if (strcmp(name, "malformed") == 0) {
        const char *bad[] = {
            "doc\tsave\tsource\tactor\t-1\t2", "doc\tsave\tsource\tactor\t1x\t2",
            "doc\tsave\tsource\tactor\t18446744073709551616\t2",
            "doc\tsave\tsource\tactor\t1\t2\textra", "doc\tsave\tsource\tactor\t\t2",
            "doc\tsave\tsource\tactor\t1", "\tsave\tsource\tactor\t1\t2",
            "doc\tsave\tsource\tbad\nactor\t1\t2"};
        memset(&loaded, 0xa5, sizeof loaded);
        unsigned char before[sizeof loaded]; memcpy(before, &loaded, sizeof loaded);
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i) {
            OK(umi_repository_save(&first.repository, "00000000000000000001", bad[i]));
            CHECK(umi_lineage_load(&first, 1U, &loaded) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(before, &loaded, sizeof loaded) == 0);
        }
        char overlong[256]; memset(overlong, 'e', 128U);
        strcpy(overlong + 128U, "\tsave\tsource\tactor\t1\t2");
        OK(umi_repository_save(&first.repository, "00000000000000000001", overlong));
        CHECK(umi_lineage_load(&first, 1U, &loaded) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, &loaded, sizeof loaded) == 0);
    } else if (strcmp(name, "exhaustion") == 0) {
        first.next_sequence = UINT64_MAX;
        OK(umi_lineage_append(&first, &record));
        CHECK(record.sequence == UINT64_MAX && first.next_sequence == 0U);
        CHECK(umi_lineage_append(&first, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        first.next_sequence = UINT64_MAX;
        CHECK(umi_lineage_append(&first, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        OK(umi_lineage_load(&first, UINT64_MAX, &loaded));
        CHECK(loaded.sequence == UINT64_MAX);
    } else {
        return 2;
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) { return 2; }
    UmiDataServer *server = NULL;
    OK(umi_data_server_create_memory(&server));
    int result = exercise(server, argv[1]);
    umi_data_server_destroy(server);
    return result;
}
