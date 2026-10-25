/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_bundle_export.c
 *
 * PURPOSE:
 *   Export must bind the saved head before touching the selected output file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Export must bind the saved head before touching the selected output file. */
#include "../build_log/fixture.h"
#include "umicom/data/record_bundle_files.h"
#include "umicom/platform/rooted_files.h"
int main(int argc, char **argv) {
  CHECK(argc == 2 || argc == 3);
  const char *name = argv[1];
  const char *cases[] = {"roundtrip", "stale",    "metadata",  "digest",         "new",
                         "null",      "relative", "exclusive", "nested",         "capacity",
                         "missing",   "binary",   "later",     "missing-content"};
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(path, directory, "reviewed.umibundle");
  UmiDataServer *store = NULL;
  CHECK((argc == 3 ? umi_data_server_create_sqlite(":memory:", &store)
                   : umi_data_server_create_memory(&store)) == UMI_STATUS_OK);
  UmiRecordAmendment author = {.entity_id = "export.record",
                               .source = "Synthetic example",
                               .actor = "Example author",
                               .reason = "File export fixture",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision first, reviewed, saved;
  unsigned char content[UMI_RECORD_DOCUMENT_MAX_BYTES] = {0, 7, 255, 0, 8};
  size_t content_length = strcmp(name, "binary") == 0 ? sizeof(content) : 5;
  if (content_length == sizeof(content)) {
    for (size_t i = 5; i < content_length; ++i)
      content[i] = (unsigned char)(i % 256U);
  }
  CHECK(umi_record_document_append(store, &author, content, content_length, 0, &first) ==
        UMI_STATUS_OK);
  author.recorded_at_nanoseconds = 2;
  CHECK(umi_record_document_append(store, &author, "latest", 6, 1, &reviewed) == UMI_STATUS_OK);
  UmiRecordRevision expected = reviewed;
  UmiStatus wanted = UMI_STATUS_OK;
  if (strcmp(name, "stale") == 0) {
    expected = first;
    wanted = UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(name, "metadata") == 0) {
    memcpy(expected.amendment.actor, "Another", 8);
    wanted = UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(name, "digest") == 0) {
    expected.amendment.content_sha256[0] = expected.amendment.content_sha256[0] == 'a' ? 'b' : 'a';
    wanted = UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(name, "new") == 0) {
    expected.revision = 0;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "missing") == 0) {
    memcpy(expected.amendment.entity_id, "missing.record", 15);
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (strcmp(name, "nested") == 0) {
    CHECK(umi_data_server_begin(store) == UMI_STATUS_OK);
    wanted = UMI_STATUS_BUSY;
  }
  if (strcmp(name, "capacity") == 0) {
    for (uint64_t i = 2; i <= UMI_RECORD_BUNDLE_MAX_REVISIONS; ++i) {
      author.recorded_at_nanoseconds = i + 1;
      CHECK(umi_record_document_append(store, &author, "x", 1, i, &expected) == UMI_STATUS_OK);
    }
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (strcmp(name, "missing-content") == 0) {
    author = reviewed.amendment;
    author.recorded_at_nanoseconds = 3;
    CHECK(umi_record_history_append(store, &author, 2, &expected) == UMI_STATUS_OK);
    /* A metadata-only link cannot be exported as if it contained payload. */
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_record_bundle_export_reviewed(NULL, &expected, path) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_bundle_export_reviewed(store, NULL, path) == UMI_STATUS_INVALID_ARGUMENT);
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "relative") == 0)
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  const char *destination = strcmp(name, "relative") == 0 ? "relative.umibundle" : path;
  UmiStatus status = umi_record_bundle_export_reviewed(
      store, strcmp(name, "null") == 0 ? NULL : &expected, destination);
  CHECK(status == wanted);
  if (strcmp(name, "nested") == 0) {
    CHECK(umi_data_server_in_transaction(store));
    CHECK(umi_data_server_rollback(store) == UMI_STATUS_OK);
  }
  if (status != UMI_STATUS_OK) {
    int exists = 1;
    CHECK(UmiRootedFileExists(directory, "reviewed.umibundle", &exists) == UMI_STATUS_OK &&
          !exists);
  } else {
    if (strcmp(name, "exclusive") == 0) {
      CHECK(umi_record_bundle_export_reviewed(store, &reviewed, path) == UMI_STATUS_ALREADY_EXISTS);
    }
    if (strcmp(name, "later") == 0) {
      author.recorded_at_nanoseconds = 3;
      CHECK(umi_record_document_append(store, &author, "after export", 12, 2, &saved) ==
            UMI_STATUS_OK);
    }
    UmiRecordBundle *bundle = NULL;
    CHECK(umi_record_bundle_read(directory, "reviewed.umibundle", &bundle) == UMI_STATUS_OK);
    CHECK(umi_record_bundle_count(bundle) == 2);
    UmiRecordDocument *document = malloc(sizeof(*document));
    CHECK(document);
    CHECK(umi_record_bundle_at(bundle, 0, document) == UMI_STATUS_OK);
    CHECK(document->length == content_length &&
          memcmp(document->content, content, content_length) == 0);
    CHECK(umi_record_bundle_at(bundle, 1, document) == UMI_STATUS_OK);
    CHECK(document->record.revision == 2 && document->length == 6 &&
          memcmp(document->content, "latest", 6) == 0);
    free(document);
    umi_record_bundle_destroy(bundle);
  }
  umi_data_server_destroy(store);
  return 0;
}
