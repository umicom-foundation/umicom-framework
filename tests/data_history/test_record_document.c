/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_document.c
 *
 * PURPOSE:
 *   Storage regressions use fictional records and arbitrary bytes. They exercise
 *   the same contract for each backend without opening application windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Storage regressions use fictional records and arbitrary bytes. They exercise
 * the same contract for each backend without opening application windows. */
#include "umicom/data/record_document.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
#define CONTENT_KEY "repo.record-content.61.00000000000000000001."

static UmiRecordAmendment metadata(const char *entity)
{
    UmiRecordAmendment value = {.source = "fictional-source", .actor = "test-reviewer",
        .reason = "Keep an example", .recorded_at_nanoseconds = 100U};
    (void)snprintf(value.entity_id, sizeof value.entity_id, "%s", entity);
    return value;
}

static int exercise(UmiDataServer *server, const char *name)
{
    UmiRecordAmendment m = metadata("a");
    UmiRecordRevision revision;
    UmiRecordDocument document;
    if (strcmp(name, "roundtrip") == 0) {
        const char original[] = "First review\nSecond line\twith a tab";
        OK(umi_record_document_append(server, &m, original, sizeof original - 1U, 0U, &revision));
        CHECK(revision.revision == 1U && m.content_sha256[0] == '\0');
        strcpy(m.reason, "Correct an example");
        OK(umi_record_document_append(server, &m, "amended", 7U, 1U, &revision));
        OK(umi_record_document_load(server, "a", 1U, &document));
        CHECK(document.length == sizeof original - 1U);
        CHECK(memcmp(document.content, original, document.length) == 0);
        CHECK(strcmp(document.record.amendment.reason, "Keep an example") == 0);
        OK(umi_record_document_latest(server, "a", &document));
        CHECK(document.record.revision == 2U && document.length == 7U);
        CHECK(memcmp(document.content, "amended", 7U) == 0);
        uint64_t count = 0U; OK(umi_record_document_verify(server, "a", &count)); CHECK(count == 2U);
        OK(umi_record_history_verify(server, "a", &count)); CHECK(count == 2U);
    } else if (strcmp(name, "boundaries") == 0) {
        unsigned char data[UMI_RECORD_DOCUMENT_MAX_BYTES];
        for (size_t i = 0U; i < sizeof data; ++i) data[i] = (unsigned char)(i % 256U);
        const size_t lengths[] = {0U, 1U, 1023U, 1024U, 1025U, UMI_RECORD_DOCUMENT_MAX_BYTES};
        uint64_t expected = 0U;
        for (size_t i = 0U; i < sizeof lengths / sizeof lengths[0]; ++i) {
            OK(umi_record_document_append(server, &m, lengths[i] ? data : NULL, lengths[i], expected, &revision));
            expected = revision.revision;
            OK(umi_record_document_latest(server, "a", &document));
            CHECK(document.length == lengths[i] && memcmp(document.content, data, document.length) == 0);
            for (size_t j = document.length; j < sizeof document.content; ++j) CHECK(document.content[j] == 0U);
            if (lengths[i] == 0U) CHECK(strcmp(revision.amendment.content_sha256,
                "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0);
        }
        uint64_t count = 0U; OK(umi_record_document_verify(server, "a", &count)); CHECK(count == expected);
    } else if (strcmp(name, "digest") == 0) {
        memset(m.content_sha256, 'x', sizeof m.content_sha256);
        OK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision));
        CHECK(strcmp(revision.amendment.content_sha256,
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0);
        OK(umi_record_document_latest(server, "a", &document)); CHECK(document.length == 3U);
    } else if (strcmp(name, "stale") == 0) {
        OK(umi_record_document_append(server, &m, "old", 3U, 0U, &revision));
        unsigned char before[sizeof revision]; memcpy(before, &revision, sizeof revision);
        CHECK(umi_record_document_append(server, &m, "new", 3U, 0U, &revision) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, &revision, sizeof revision) == 0);
        CHECK(umi_record_document_append(server, &m, "new", 3U, 2U, &revision) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_document_latest(server, "a", &document)); CHECK(document.record.revision == 1U);
        CHECK(memcmp(document.content, "old", 3U) == 0);
    } else if (strcmp(name, "nested") == 0) {
        OK(umi_data_server_begin(server));
        OK(umi_data_server_set(server, "owner-pending", "retained"));
        CHECK(umi_record_document_append(server, &m, "x", 1U, 0U, &revision) == UMI_STATUS_BUSY);
        CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_BUSY);
        CHECK(umi_record_document_load(server, "a", 1U, &document) == UMI_STATUS_BUSY);
        uint64_t count = 77U; CHECK(umi_record_document_verify(server, "a", &count) == UMI_STATUS_BUSY);
        CHECK(count == 77U && umi_data_server_in_transaction(server));
        OK(umi_data_server_commit(server));
        char text[16]; OK(umi_data_server_get(server, "owner-pending", text, sizeof text)); CHECK(strcmp(text, "retained") == 0);
    } else if (strcmp(name, "invalid") == 0) {
        memset(&revision, 0xa5, sizeof revision);
        unsigned char before[sizeof revision]; memcpy(before, &revision, sizeof revision);
        CHECK(umi_record_document_append(server, &m, NULL, 1U, 0U, &revision) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_document_append(server, &m, "x", UMI_RECORD_DOCUMENT_MAX_BYTES + 1U, 0U, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_record_document_append(server, &m, "x", 1U, UINT64_MAX, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        m.actor[0] = '\0'; CHECK(umi_record_document_append(server, &m, "x", 1U, 0U, &revision) == UMI_STATUS_INVALID_ARGUMENT);
        m = metadata("a"); strcpy(m.reason, "bad\nreason");
        CHECK(umi_record_document_append(server, &m, "x", 1U, 0U, &revision) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &revision, sizeof revision) == 0);
        size_t count; OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 0U);
    } else if (strcmp(name, "isolation") == 0) {
        const char *names[] = {"a", "a/b", "a.b", "a:b", "61"};
        for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i) {
            m = metadata(names[i]); OK(umi_record_document_append(server, &m, names[i], strlen(names[i]), 0U, &revision));
        }
        for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i) {
            OK(umi_record_document_latest(server, names[i], &document));
            CHECK(document.length == strlen(names[i]) && memcmp(document.content, names[i], document.length) == 0);
        }
        memset(m.entity_id, 'z', sizeof m.entity_id - 1U); m.entity_id[63] = '\0';
        OK(umi_record_document_append(server, &m, "max-id", 6U, 0U, &revision));
        OK(umi_record_document_latest(server, m.entity_id, &document)); CHECK(document.length == 6U);
    } else if (strcmp(name, "tamper") == 0) {
        OK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision));
        OK(umi_data_server_set(server, CONTENT_KEY "01", "616264"));
        memset(&document, 0xa5, sizeof document);
        unsigned char before[sizeof document]; memcpy(before, &document, sizeof document);
        CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, &document, sizeof document) == 0);
        uint64_t count = 77U; CHECK(umi_record_document_verify(server, "a", &count) == UMI_STATUS_PARSE_ERROR); CHECK(count == 77U);
        CHECK(umi_record_document_append(server, &m, "new", 3U, 1U, &revision) == UMI_STATUS_PARSE_ERROR);
        OK(umi_record_history_latest(server, "a", &revision)); CHECK(revision.revision == 1U);
    } else if (strcmp(name, "manifest") == 0) {
        OK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision));
        const char *bad[] = {"UMID2\t3\t1", "UMID1\t3\t2", "UMID1\t16385\t17", "UMID1\t03\t1",
            "UMID1\t3\t01", "UMID1\t-3\t1", "UMID1\t3\t1\textra", "UMID1\t18446744073709551616\t1"};
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i) {
            OK(umi_data_server_set(server, CONTENT_KEY "00", bad[i]));
            CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_PARSE_ERROR);
        }
        OK(umi_data_server_set(server, CONTENT_KEY "00", "UMID1\t3\t1"));
        OK(umi_record_document_latest(server, "a", &document)); CHECK(document.length == 3U);
    } else if (strcmp(name, "chunks") == 0) {
        OK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision));
        const char *bad[] = {"6162", "61626300", "61626G", "6162FF", ""};
        for (size_t i = 0U; i < sizeof bad / sizeof bad[0]; ++i) {
            OK(umi_data_server_set(server, CONTENT_KEY "01", bad[i]));
            CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_PARSE_ERROR);
        }
        OK(umi_data_server_delete(server, CONTENT_KEY "01"));
        CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_record_document_append(server, &m, "new", 3U, 1U, &revision) == UMI_STATUS_NOT_FOUND);
        OK(umi_record_history_latest(server, "a", &revision)); CHECK(revision.revision == 1U);
    } else if (strcmp(name, "mixed-history") == 0) {
        strcpy(m.content_sha256, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        OK(umi_record_history_append(server, &m, 0U, &revision));
        CHECK(umi_record_document_append(server, &m, "new", 3U, 1U, &revision) == UMI_STATUS_NOT_FOUND);
        OK(umi_record_history_latest(server, "a", &revision)); CHECK(revision.revision == 1U);
        CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "occupied") == 0) {
        OK(umi_data_server_set(server, CONTENT_KEY "01", "retained"));
        CHECK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_history_latest(server, "a", &revision) == UMI_STATUS_NOT_FOUND);
        char text[32]; CHECK(umi_data_server_get(server, CONTENT_KEY "00", text, sizeof text) == UMI_STATUS_NOT_FOUND);
        OK(umi_data_server_get(server, CONTENT_KEY "01", text, sizeof text)); CHECK(strcmp(text, "retained") == 0);
    } else if (strcmp(name, "missing") == 0) {
        memset(&document, 0xa5, sizeof document);
        unsigned char before[sizeof document]; memcpy(before, &document, sizeof document);
        CHECK(umi_record_document_latest(server, "a", &document) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_record_document_load(server, "a", 1U, &document) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_record_document_load(server, "a", 0U, &document) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &document, sizeof document) == 0);
        uint64_t count = 77U; CHECK(umi_record_document_verify(server, "a", &count) == UMI_STATUS_NOT_FOUND); CHECK(count == 77U);
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(umi_data_server_backend(server) == UMI_DATA_BACKEND_MEMORY);
        for (size_t i = 0U; i < 2045U; ++i) {
            char key[40]; (void)snprintf(key, sizeof key, "fixture.%zu", i); OK(umi_data_server_set(server, key, "kept"));
        }
        CHECK(umi_record_document_append(server, &m, "abc", 3U, 0U, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_record_history_latest(server, "a", &revision) == UMI_STATUS_NOT_FOUND);
        size_t count; OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 2045U);
    } else { return 2; }
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
    CHECK(status == UMI_STATUS_OK);
    int result = exercise(server, argv[2]);
    umi_data_server_destroy(server);
    return result;
}
