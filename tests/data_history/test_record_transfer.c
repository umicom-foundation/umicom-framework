/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_transfer.c
 *
 * PURPOSE:
 *   Original history and import provenance must commit or roll back together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Original history and import provenance must commit or roll back together. */
#include "../../src/data/include/record_history_internal.h"
#include "umicom/data/record_transfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  const char *cases[] = {"restore",
                         "receipt-collision",
                         "record-collision",
                         "receipt-orphan",
                         "nested",
                         "wrong-digest",
                         "invalid-id",
                         "invalid-time",
                         "reserved",
                         "repeat",
                         "later",
                         "altered-history",
                         "altered-content",
                         "amended-receipt",
                         "missing-receipt",
                         "other-entity",
                         "write-failure",
                         "automatic-rollback",
                         "commit-failure"};
  if (argc != 2 && argc != 3)
    return 2;
  const char *name = argv[1];
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  int result = 0;
  UmiDataServer *source = NULL, *target = NULL;
  UmiRecordBundle *bundle = NULL;
  UmiRecordDocument *document = malloc(sizeof(*document));
  unsigned char *bytes = NULL;
  size_t length = 0;
  CHECK(document);
  OK(umi_data_server_create_memory(&source));
  OK(argc == 3 ? umi_data_server_create_sqlite(":memory:", &target)
               : umi_data_server_create_memory(&target));
  UmiRecordAmendment author = {.entity_id = "transfer.example",
                               .source = "Original source",
                               .actor = "Original author",
                               .reason = "Original edit",
                               .recorded_at_nanoseconds = 1};
  if (strcmp(name, "reserved") == 0)
    memcpy(author.entity_id, "transfer.receipt.source", sizeof("transfer.receipt.source"));
  UmiRecordRevision saved;
  OK(umi_record_document_append(source, &author, "first", 5, 0, &saved));
  author.recorded_at_nanoseconds = 2;
  OK(umi_record_document_append(source, &author, "second", 6, 1, &saved));
  OK(umi_record_bundle_capture(source, author.entity_id, &bundle));
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  char digest[65] = {0};
  memcpy(digest, bytes + length - 64, 64);
  UmiRecordTransferRequest request = {.receipt_id = "example-1",
                                      .attribution = {.source = "Import workstation",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed transfer",
                                                      .recorded_at_nanoseconds = 3}};
  UmiRecordTransferReceipt output, loaded, sentinel;
  memset(&sentinel, 0x5a, sizeof(sentinel));
  output = sentinel;
  UmiStatus refused = UMI_STATUS_OK;
  if (strcmp(name, "wrong-digest") == 0) {
    digest[0] = digest[0] == 'a' ? 'b' : 'a';
    refused = UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(name, "invalid-id") == 0) {
    memcpy(request.receipt_id, "bad.id", 7);
    refused = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "invalid-time") == 0) {
    request.attribution.recorded_at_nanoseconds = 0;
    refused = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "reserved") == 0)
    refused = UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(name, "receipt-collision") == 0 || strcmp(name, "record-collision") == 0 ||
      strcmp(name, "other-entity") == 0) {
    UmiRecordAmendment occupied = author;
    const char *id =
        strcmp(name, "receipt-collision") == 0
            ? "transfer.receipt.example-1"
            : (strcmp(name, "other-entity") == 0 ? "unrelated.record" : author.entity_id);
    memcpy(occupied.entity_id, id, strlen(id) + 1);
    OK(umi_record_document_append(target, &occupied, "retained", 8, 0, &saved));
    if (strcmp(name, "other-entity") != 0)
      refused = UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(name, "receipt-orphan") == 0) {
    char key[192];
    OK(umi_record_history_key("transfer.receipt.example-1", 9, key, sizeof(key)));
    OK(umi_data_server_set(target, key, "orphan retained"));
    refused = UMI_STATUS_INVALID_STATE;
  }
  bool fault = strcmp(name, "write-failure") == 0 || strcmp(name, "automatic-rollback") == 0 ||
               strcmp(name, "commit-failure") == 0;
  if (fault) {
    CHECK(argc == 3);
    OK(umi_data_server_set(target, "unrelated.key", "retained"));
    char key[192], sql[1200];
    OK(umi_record_history_key("transfer.receipt.example-1", 1, key, sizeof(key)));
    int written;
    if (strcmp(name, "commit-failure") == 0)
      written =
          snprintf(sql, sizeof(sql),
                   "PRAGMA foreign_keys=ON; CREATE TABLE allowed(id INTEGER PRIMARY KEY); CREATE "
                   "TABLE deferred_row(id INTEGER REFERENCES allowed(id) DEFERRABLE INITIALLY "
                   "DEFERRED); CREATE TRIGGER fail_receipt AFTER INSERT ON umicom_kv WHEN "
                   "NEW.key='%s' BEGIN INSERT INTO deferred_row VALUES(42); END;",
                   key);
    else
      written = snprintf(sql, sizeof(sql),
                         "CREATE TRIGGER fail_receipt BEFORE INSERT ON umicom_kv WHEN NEW.key='%s' "
                         "BEGIN SELECT RAISE(%s,'receipt refused'); END;",
                         key, strcmp(name, "automatic-rollback") == 0 ? "ROLLBACK" : "ABORT");
    CHECK(written > 0 && (size_t)written < sizeof(sql));
    OK(umi_data_server_execute(target, sql));
    refused = UMI_STATUS_IO_ERROR;
  }
  if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(target));
    refused = UMI_STATUS_BUSY;
  }
  size_t before = umi_data_server_count(target);
  UmiStatus status = umi_record_transfer_restore(target, bundle, digest, &request, &output);
  CHECK(status == refused);
  if (refused != UMI_STATUS_OK) {
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
    CHECK(umi_data_server_count(target) == before);
    if (strcmp(name, "nested") == 0) {
      CHECK(umi_data_server_in_transaction(target));
      OK(umi_data_server_rollback(target));
    } else
      CHECK(!umi_data_server_in_transaction(target));
    if (fault) {
      OK(umi_data_server_execute(target, "DROP TRIGGER fail_receipt;"));
      OK(umi_record_transfer_restore(target, bundle, digest, &request, &output));
    } else
      goto cleanup;
  }
  CHECK(output.imported_head.revision == 2 && output.receipt.revision == 1);
  CHECK(strcmp(output.imported_head.amendment.actor, "Original author") == 0);
  CHECK(strcmp(output.receipt.amendment.actor, "Import operator") == 0);
  OK(umi_record_transfer_receipt_load(target, "example-1", &loaded));
  CHECK(strcmp(loaded.bundle_sha256, digest) == 0);
  OK(umi_record_transfer_receipt_verify(target, "example-1", &loaded));
  if (strcmp(name, "repeat") == 0) {
    before = umi_data_server_count(target);
    output = sentinel;
    CHECK(umi_record_transfer_restore(target, bundle, digest, &request, &output) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0 &&
          umi_data_server_count(target) == before);
  }
  if (strcmp(name, "later") == 0) {
    author.recorded_at_nanoseconds = 4;
    OK(umi_record_document_append(target, &author, "later", 5, 2, &saved));
    OK(umi_record_transfer_receipt_verify(target, "example-1", &loaded));
    CHECK(loaded.imported_head.revision == 2);
  }
  if (strcmp(name, "altered-history") == 0) {
    UmiRecordRevision first;
    char key[192], text[UMI_RECORD_REVISION_TEXT_CAPACITY];
    OK(umi_record_history_load(target, author.entity_id, 1, &first));
    memcpy(first.amendment.actor, "Changed author", sizeof("Changed author"));
    OK(umi_record_revision_format(&first, text, sizeof(text)));
    OK(umi_record_history_key(author.entity_id, 1, key, sizeof(key)));
    OK(umi_data_server_set(target, key, text));
    output = sentinel;
    CHECK(umi_record_transfer_receipt_verify(target, "example-1", &output) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
    OK(umi_record_transfer_receipt_load(target, "example-1", &loaded));
  }
  if (strcmp(name, "altered-content") == 0) {
    char head[192], key[256];
    OK(umi_record_history_key(author.entity_id, 0, head, sizeof(head)));
    int written = snprintf(key, sizeof(key), "repo.record-content.%s.00000000000000000001.00",
                           head + strlen("repo.record-history.h."));
    CHECK(written > 0 && (size_t)written < sizeof(key));
    OK(umi_data_server_set(target, key, "malformed manifest"));
    output = sentinel;
    CHECK(umi_record_transfer_receipt_verify(target, "example-1", &output) != UMI_STATUS_OK);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
    OK(umi_record_transfer_receipt_load(target, "example-1", &loaded));
  }
  if (strcmp(name, "amended-receipt") == 0) {
    UmiRecordAmendment edited = output.receipt.amendment;
    edited.recorded_at_nanoseconds = 4;
    OK(umi_record_document_append(target, &edited, "changed receipt", 15, 1, &saved));
    output = sentinel;
    CHECK(umi_record_transfer_receipt_load(target, "example-1", &output) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(umi_record_transfer_receipt_verify(target, "example-1", &output) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
  }
  if (strcmp(name, "missing-receipt") == 0) {
    output = sentinel;
    CHECK(umi_record_transfer_receipt_load(target, "absent", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&output, &sentinel, sizeof(output)) == 0);
  }
  if (strcmp(name, "other-entity") == 0) {
    OK(umi_record_document_latest(target, "unrelated.record", document));
    CHECK(document->length == 8 && memcmp(document->content, "retained", 8) == 0);
  }
cleanup:
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(target);
  umi_data_server_destroy(source);
  free(document);
  return result;
}
