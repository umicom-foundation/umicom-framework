/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_catalogue.c
 *
 * PURPOSE:
 *   Backend-independent discovery fixtures use fictional administrative records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Backend-independent discovery fixtures use fictional administrative records. */
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_document.h"
#include "../../src/data/include/record_history_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static int append(UmiDataServer *server, const char *entity, uint64_t expected, const char *actor)
{
    UmiRecordAmendment a = {.source="Synthetic catalogue", .reason="Reviewed fixture", .recorded_at_nanoseconds=expected+1U};
    (void)snprintf(a.entity_id, sizeof a.entity_id, "%s", entity);
    (void)snprintf(a.actor, sizeof a.actor, "%s", actor);
    UmiRecordRevision saved;
    OK(umi_record_document_append(server, &a, "fictional", 9U, expected, &saved));
    return 0;
}
static int exercise(UmiDataServer *server, const char *name)
{
    UmiRecordCatalogueQuery query = {.entity_prefix="notes.", .limit=2U};
    UmiRecordCataloguePage page, previous;
    memset(&page, 0xa5, sizeof page); previous = page;
    if (strcmp(name, "empty") == 0) {
        OK(umi_record_catalogue_page(server, &query, &page));
        CHECK(page.count == 0U && page.matching_count == 0U && strlen(page.snapshot) == 64U);
        CHECK(page.next_after_entity[0] == '\0'); return 0;
    }
    if (strcmp(name, "scan-limit") == 0) {
        CHECK(umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE);
        OK(umi_data_server_execute(server,
            "WITH RECURSIVE n(x) AS (VALUES(1) UNION ALL SELECT x+1 FROM n WHERE x<32769) "
            "INSERT INTO umicom_kv(key,value) SELECT 'catalogue-fixture.'||x,'bounded' FROM n;"));
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&page, &previous, sizeof page) == 0);
        CHECK(!umi_data_server_in_transaction(server)); return 0;
    }
    CHECK(append(server, "notes.c", 0U, "Carol") == 0);
    CHECK(append(server, "notes.a", 0U, "Alice") == 0);
    CHECK(append(server, "notes.b", 0U, "Bob") == 0);
    CHECK(append(server, "elsewhere.a", 0U, "Alice") == 0);
    if (strcmp(name, "pages") == 0) {
        OK(umi_record_catalogue_page(server, &query, &page));
        CHECK(page.namespace_count == 3U && page.matching_count == 3U && page.count == 2U);
        CHECK(strcmp(page.rows[0].amendment.entity_id, "notes.a") == 0);
        CHECK(strcmp(page.rows[1].amendment.entity_id, "notes.b") == 0);
        memcpy(query.expected_snapshot, page.snapshot, sizeof query.expected_snapshot);
        memcpy(query.after_entity, page.next_after_entity, sizeof query.after_entity);
        OK(umi_record_catalogue_page(server, &query, &page));
        CHECK(page.count == 1U && strcmp(page.rows[0].amendment.entity_id, "notes.c") == 0);
        CHECK(page.next_after_entity[0] == '\0');
        memcpy(query.after_entity, "notes.c", sizeof "notes.c");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.count == 0U);
    } else if (strcmp(name, "search") == 0) {
        memcpy(query.text, "Alice", sizeof "Alice");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.count == 1U);
        CHECK(strcmp(page.rows[0].amendment.entity_id, "notes.a") == 0);
        memcpy(query.text, "alice", sizeof "alice");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.count == 0U);
        memcpy(query.text, "Reviewed", sizeof "Reviewed");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.matching_count == 3U);
        memcpy(query.text, "Synthetic", sizeof "Synthetic");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.matching_count == 3U);
        memcpy(query.text, "fictional", sizeof "fictional");
        OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.count == 0U);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(umi_record_catalogue_page(NULL, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_catalogue_page(server, NULL, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_catalogue_page(server, &query, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        query.limit = 0U; CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        query.limit = 17U; CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        query.limit = 1U; memset(query.text, 'x', sizeof query.text);
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        memset(query.text, 0, sizeof query.text); query.text[0] = '\n';
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        query.text[0] = '\0'; query.after_entity[0] = 'a';
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&page, &previous, sizeof page) == 0);
    } else if (strcmp(name, "nested") == 0) {
        OK(umi_data_server_begin(server));
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server)); CHECK(memcmp(&page, &previous, sizeof page) == 0);
        OK(umi_data_server_rollback(server));
    } else if (strcmp(name, "changed-head") == 0 || strcmp(name, "added-head") == 0 ||
               strcmp(name, "changed-query") == 0 || strcmp(name, "other-namespace") == 0 || strcmp(name, "cursor") == 0) {
        OK(umi_record_catalogue_page(server, &query, &page)); previous = page;
        memcpy(query.expected_snapshot, page.snapshot, sizeof query.expected_snapshot);
        memcpy(query.after_entity, page.next_after_entity, sizeof query.after_entity);
        if (strcmp(name, "changed-head") == 0) CHECK(append(server, "notes.a", 1U, "Alice") == 0);
        if (strcmp(name, "added-head") == 0) CHECK(append(server, "notes.d", 0U, "Dana") == 0);
        if (strcmp(name, "changed-query") == 0) memcpy(query.text, "Alice", sizeof "Alice");
        if (strcmp(name, "cursor") == 0) memcpy(query.after_entity, "notes.missing", sizeof "notes.missing");
        if (strcmp(name, "other-namespace") == 0) {
            CHECK(append(server, "elsewhere.b", 0U, "Bob") == 0);
            OK(umi_record_catalogue_page(server, &query, &page)); CHECK(page.count == 1U);
        } else {
            CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(&page, &previous, sizeof page) == 0);
        }
    } else if (strcmp(name, "bad-key") == 0 || strcmp(name, "missing-row") == 0 || strcmp(name, "bad-head") == 0) {
        char key[192]; OK(umi_record_history_key("notes.a", 0U, key, sizeof key));
        if (strcmp(name, "bad-key") == 0) OK(umi_data_server_set(server, "repo.record-history.h.00", "1"));
        else if (strcmp(name, "bad-head") == 0) OK(umi_data_server_set(server, key, "junk"));
        else { OK(umi_record_history_key("notes.a", 1U, key, sizeof key)); OK(umi_data_server_delete(server, key)); }
        UmiStatus result = umi_record_catalogue_page(server, &query, &page);
        CHECK(result == (strcmp(name, "missing-row") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_PARSE_ERROR));
        CHECK(memcmp(&page, &previous, sizeof page) == 0);
    } else if (strcmp(name, "entity-limit") == 0) {
        for (size_t i = 3U; i <= UMI_RECORD_CATALOGUE_ENTITY_LIMIT; ++i) {
            char entity[64]; (void)snprintf(entity, sizeof entity, "notes.limit.%zu", i);
            CHECK(append(server, entity, 0U, "limit") == 0);
        }
        CHECK(umi_record_catalogue_page(server, &query, &page) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&page, &previous, sizeof page) == 0);
    } else if (strcmp(name, "metadata") == 0) {
        UmiRecordRevision row; OK(umi_record_history_latest(server, "notes.a", &row));
        memcpy(row.amendment.entity_id, "notes.metadata", sizeof "notes.metadata");
        OK(umi_record_history_append(server, &row.amendment, 0U, &row));
        query.limit = 16U; OK(umi_record_catalogue_page(server, &query, &page));
        CHECK(page.count == 4U);
        UmiRecordDocument *doc = malloc(sizeof *doc); CHECK(doc != NULL);
        CHECK(umi_record_document_open_reviewed(server, &page.rows[3], doc) == UMI_STATUS_NOT_FOUND); free(doc);
    } else if (strcmp(name, "reviewed-open") == 0 || strcmp(name, "stale-open") == 0 || strcmp(name, "changed-metadata") == 0 || strcmp(name, "broken-content") == 0) {
        OK(umi_record_catalogue_page(server, &query, &page));
        UmiRecordDocument *doc = malloc(sizeof *doc), *unchanged = malloc(sizeof *unchanged);
        CHECK(doc != NULL && unchanged != NULL); memset(doc, 0xa5, sizeof *doc); *unchanged = *doc;
        if (strcmp(name, "stale-open") == 0) CHECK(append(server, "notes.a", 1U, "Alice") == 0);
        if (strcmp(name, "changed-metadata") == 0) {
            UmiRecordRevision row = page.rows[0]; memcpy(row.amendment.actor, "Changed", sizeof "Changed");
            char key[192], text[UMI_RECORD_REVISION_TEXT_CAPACITY];
            OK(umi_record_history_key("notes.a", 1U, key, sizeof key));
            OK(umi_record_revision_format(&row, text, sizeof text)); OK(umi_data_server_set(server, key, text));
        }
        if (strcmp(name, "broken-content") == 0) {
            char history[192], content[192]; OK(umi_record_history_key("notes.a", 1U, history, sizeof history));
            (void)snprintf(content, sizeof content, "repo.record-content.%s.01", history + sizeof "repo.record-history.r." - 1U);
            OK(umi_data_server_set(server, content, "00"));
        }
        UmiStatus result = umi_record_document_open_reviewed(server, &page.rows[0], doc);
        if (strcmp(name, "reviewed-open") == 0) { CHECK(result == UMI_STATUS_OK); CHECK(doc->length == 9U && memcmp(doc->content, "fictional", 9U) == 0); }
        else { CHECK(result == (strcmp(name, "broken-content") == 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_INVALID_STATE)); CHECK(memcmp(doc, unchanged, sizeof *doc) == 0); }
        free(doc); free(unchanged);
    } else return 2;
    CHECK(!umi_data_server_in_transaction(server));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    UmiDataServer *server = NULL;
    UmiStatus status = strcmp(argv[1], "memory") == 0 ? umi_data_server_create_memory(&server) :
        strcmp(argv[1], "sqlite") == 0 ? umi_data_server_create_sqlite(":memory:", &server) : UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) return 1;
    int result = exercise(server, argv[2]); umi_data_server_destroy(server); return result;
}
