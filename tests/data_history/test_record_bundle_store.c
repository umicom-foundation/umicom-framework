/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Full entity history restores atomically without replacing occupied records. */
#include "../../src/data/include/record_history_internal.h"
#include "umicom/data/record_bundle.h"
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
  const char *cases[] = {"write-failure",   "commit-failure",     "automatic-rollback",
                         "restore",         "collision",          "orphan-rollback",
                         "nested-capture",  "nested-restore",     "missing",
                         "capacity",        "source-independent", "other-entity",
                         "large",           "cross-backend",      "repeat",
                         "future-history",  "future-content",     "malformed-content-key",
                         "neighbor-entity", "scan-limit",         "scan-boundary"};
  if (argc != 2 && argc != 3)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *source = NULL, *target = NULL;
  UmiRecordBundle *bundle = NULL, *extra = NULL;
  UmiRecordDocument *document = malloc(sizeof(*document));
  unsigned char *payload = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  CHECK(document && payload);
  memset(payload, 0x9b, UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (argc == 3) {
    OK(umi_data_server_create_sqlite(":memory:", &source));
    if (strcmp(name, "cross-backend") == 0)
      OK(umi_data_server_create_memory(&target));
    else
      OK(umi_data_server_create_sqlite(":memory:", &target));
  } else {
    OK(umi_data_server_create_memory(&source));
    OK(umi_data_server_create_memory(&target));
  }
  UmiRecordAmendment metadata = {.entity_id = "bundle.record",
                                 .source = "Synthetic source",
                                 .actor = "Example author",
                                 .reason = "Storage fixture",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  size_t length = strcmp(name, "large") == 0 ? UMI_RECORD_DOCUMENT_MAX_BYTES : 5;
  OK(umi_record_document_append(source, &metadata, payload, length, 0, &saved));
  metadata.recorded_at_nanoseconds = 2;
  OK(umi_record_document_append(source, &metadata, "second", 6, 1, &saved));
  if (strcmp(name, "capacity") == 0) {
    for (uint64_t i = 2; i < UMI_RECORD_BUNDLE_MAX_REVISIONS + 1; ++i) {
      metadata.recorded_at_nanoseconds = i + 1;
      OK(umi_record_document_append(source, &metadata, "x", 1, i, &saved));
    }
    CHECK(umi_record_bundle_capture(source, "bundle.record", &bundle) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          bundle == NULL);
    goto cleanup;
  }
  if (strcmp(name, "missing") == 0) {
    CHECK(umi_record_bundle_capture(source, "bundle.absent", &bundle) == UMI_STATUS_NOT_FOUND &&
          bundle == NULL);
    goto cleanup;
  }
  if (strcmp(name, "nested-capture") == 0) {
    OK(umi_data_server_begin(source));
    OK(umi_data_server_set(source, "caller.pending", "kept"));
    CHECK(umi_record_bundle_capture(source, "bundle.record", &bundle) == UMI_STATUS_BUSY &&
          bundle == NULL);
    char value[16];
    OK(umi_data_server_get(source, "caller.pending", value, sizeof value));
    CHECK(strcmp(value, "kept") == 0);
    OK(umi_data_server_rollback(source));
    goto cleanup;
  }
  OK(umi_record_bundle_capture(source, "bundle.record", &bundle));
  if (strcmp(name, "nested-restore") == 0) {
    OK(umi_data_server_begin(target));
    OK(umi_data_server_set(target, "caller.pending", "kept"));
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) == UMI_STATUS_BUSY);
    char value[16];
    OK(umi_data_server_get(target, "caller.pending", value, sizeof value));
    CHECK(strcmp(value, "kept") == 0);
    OK(umi_data_server_rollback(target));
    goto cleanup;
  }
  if (strcmp(name, "collision") == 0) {
    OK(umi_record_document_append(target, &metadata, "existing", 8, 0, &saved));
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) == UMI_STATUS_INVALID_STATE);
    OK(umi_record_document_latest(target, "bundle.record", document));
    CHECK(document->length == 8 && memcmp(document->content, "existing", 8) == 0);
    goto cleanup;
  }
  if (strcmp(name, "scan-limit") == 0 || strcmp(name, "scan-boundary") == 0) {
    CHECK(argc == 3);
    char sql[512];
    size_t rows = UMI_RECORD_BUNDLE_SCAN_LIMIT + (strcmp(name, "scan-limit") == 0 ? 1U : 0U);
    int written =
        snprintf(sql, sizeof(sql),
                 "WITH RECURSIVE n(x) AS (SELECT 1 UNION ALL SELECT x+1 FROM n WHERE x<%zu) INSERT "
                 "INTO umicom_kv(key,value) SELECT printf('fixture.%%08d',x),'retained' FROM n;",
                 rows);
    CHECK(written > 0 && (size_t)written < sizeof(sql));
    OK(umi_data_server_execute(target, sql));
    saved.revision = 99;
    UmiStatus scanned = umi_record_bundle_restore_new(target, bundle, &saved);
    if (strcmp(name, "scan-limit") == 0) {
      CHECK(scanned == UMI_STATUS_CAPACITY_EXCEEDED && saved.revision == 99 &&
            umi_data_server_count(target) == rows);
      CHECK(umi_record_document_latest(target, "bundle.record", document) == UMI_STATUS_NOT_FOUND);
    } else {
      OK(scanned);
      CHECK(saved.revision == 2);
    }
    CHECK(!umi_data_server_in_transaction(target));
    goto cleanup;
  }
  if (strcmp(name, "future-history") == 0 || strcmp(name, "future-content") == 0 ||
      strcmp(name, "malformed-content-key") == 0) {
    char key[192], history[192];
    OK(umi_record_history_key("bundle.record", 99, history, sizeof(history)));
    if (strcmp(name, "future-history") == 0)
      memcpy(key, history, strlen(history) + 1);
    else {
      const char *encoded = history + sizeof("repo.record-history.r.") - 1;
      int written = snprintf(key, sizeof(key), "repo.record-content.%s.%s", encoded,
                             strcmp(name, "future-content") == 0 ? "16" : "broken");
      CHECK(written > 0 && (size_t)written < sizeof(key));
    }
    OK(umi_data_server_set(target, key, "retained orphan"));
    saved.revision = 99;
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) == UMI_STATUS_INVALID_STATE &&
          saved.revision == 99);
    CHECK(umi_data_server_count(target) == 1 && !umi_data_server_in_transaction(target));
    char value[32];
    OK(umi_data_server_get(target, key, value, sizeof(value)));
    CHECK(strcmp(value, "retained orphan") == 0);
    CHECK(umi_record_document_latest(target, "bundle.record", document) == UMI_STATUS_NOT_FOUND);
    goto cleanup;
  }
  if (strcmp(name, "neighbor-entity") == 0) {
    UmiRecordAmendment neighbor = metadata;
    memcpy(neighbor.entity_id, "bundle.record.more", sizeof("bundle.record.more"));
    OK(umi_record_document_append(target, &neighbor, "neighbor", 8, 0, &saved));
  }
  if (strcmp(name, "orphan-rollback") == 0) {
    char key[192];
    OK(umi_record_history_key("bundle.record", 2, key, sizeof key));
    OK(umi_data_server_set(target, key, "occupied orphan"));
    size_t before = umi_data_server_count(target);
    saved.revision = 99;
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) != UMI_STATUS_OK &&
          saved.revision == 99);
    CHECK(umi_data_server_count(target) == before);
    CHECK(umi_record_document_latest(target, "bundle.record", document) == UMI_STATUS_NOT_FOUND);
    char value[32];
    OK(umi_data_server_get(target, key, value, sizeof value));
    CHECK(strcmp(value, "occupied orphan") == 0);
    goto cleanup;
  }
  if (strcmp(name, "write-failure") == 0 || strcmp(name, "commit-failure") == 0 ||
      strcmp(name, "automatic-rollback") == 0) {
    CHECK(argc == 3);
    OK(umi_data_server_set(target, "unrelated.key", "retained"));
    const char *sql =
        strcmp(name, "commit-failure") == 0
            ? "PRAGMA foreign_keys=ON; CREATE TABLE allowed(id INTEGER PRIMARY KEY); CREATE TABLE "
              "deferred_row(id INTEGER REFERENCES allowed(id) DEFERRABLE INITIALLY DEFERRED); "
              "CREATE TRIGGER fail_bundle AFTER INSERT ON umicom_kv WHEN NEW.key LIKE "
              "'repo.record-content.%.00' BEGIN INSERT INTO deferred_row VALUES(42); END;"
            : (strcmp(name, "automatic-rollback") == 0
                   ? "CREATE TRIGGER fail_bundle BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE "
                     "'repo.record-content.%.00000000000000000002.00' BEGIN SELECT RAISE(ROLLBACK, "
                     "'bundle transaction aborted'); END;"
                   : "CREATE TRIGGER fail_bundle BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE "
                     "'repo.record-content.%.00000000000000000002.00' BEGIN SELECT RAISE(ABORT, "
                     "'second revision refused'); END;");
    OK(umi_data_server_execute(target, sql));
    saved.revision = 99;
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) == UMI_STATUS_IO_ERROR &&
          saved.revision == 99);
    CHECK(!umi_data_server_in_transaction(target));
    CHECK(umi_data_server_count(target) == 1);
    CHECK(umi_record_document_latest(target, "bundle.record", document) == UMI_STATUS_NOT_FOUND);
    char value[32];
    OK(umi_data_server_get(target, "unrelated.key", value, sizeof value));
    CHECK(strcmp(value, "retained") == 0);
    OK(umi_data_server_execute(target, "DROP TRIGGER fail_bundle;"));
    OK(umi_record_bundle_restore_new(target, bundle, &saved));
    CHECK(saved.revision == 2);
    goto cleanup;
  }
  if (strcmp(name, "source-independent") == 0) {
    metadata.recorded_at_nanoseconds = 3;
    OK(umi_record_document_append(source, &metadata, "later", 5, 2, &saved));
  }
  if (strcmp(name, "other-entity") == 0) {
    UmiRecordAmendment other = metadata;
    memcpy(other.entity_id, "bundle.other", 13);
    OK(umi_record_document_append(target, &other, "retained", 8, 0, &saved));
  }
  OK(umi_record_bundle_restore_new(target, bundle, &saved));
  CHECK(saved.revision == 2);
  OK(umi_record_document_load(target, "bundle.record", 1, document));
  CHECK(document->length == length && memcmp(document->content, payload, length) == 0);
  OK(umi_record_document_latest(target, "bundle.record", document));
  CHECK(document->record.revision == 2 && document->length == 6 &&
        memcmp(document->content, "second", 6) == 0);
  uint64_t revisions = 0;
  OK(umi_record_document_verify(target, "bundle.record", &revisions));
  CHECK(revisions == 2);
  if (strcmp(name, "other-entity") == 0) {
    OK(umi_record_document_latest(target, "bundle.other", document));
    CHECK(document->length == 8 && memcmp(document->content, "retained", 8) == 0);
  }
  if (strcmp(name, "repeat") == 0)
    CHECK(umi_record_bundle_restore_new(target, bundle, &saved) == UMI_STATUS_INVALID_STATE);
  if (strcmp(name, "source-independent") == 0) {
    OK(umi_record_document_latest(source, "bundle.record", document));
    CHECK(document->record.revision == 3);
  }
  if (strcmp(name, "neighbor-entity") == 0) {
    OK(umi_record_document_latest(target, "bundle.record.more", document));
    CHECK(document->length == 8 && memcmp(document->content, "neighbor", 8) == 0);
  }
cleanup:
  umi_record_bundle_destroy(extra);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(target);
  umi_data_server_destroy(source);
  free(payload);
  free(document);
  return result;
}
