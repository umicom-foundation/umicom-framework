/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_data_readonly_open.c
 *
 * PURPOSE:
 *   Inspection must not create stores or repair an unrelated schema.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Inspection must not create stores or repair an unrelated schema. */
#include "readonly_fixture.h"
int main(int argc, char **argv) {
  const char *cases[] = {"valid",   "missing",     "empty",       "null",       "special",
                         "uri",     "long",        "corrupt",     "unrelated",  "view",
                         "columns", "null-output", "memory-mode", "unavailable",
                         "writable-valid", "writable-missing", "writable-corrupt", "writable-unrelated",
                         "writable-view", "writable-columns", "writable-uri", "writable-null-output",
                         "writable-unavailable"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  /* Run the same schema admission vectors through both access modes. Their
   * only positive difference is whether an admitted caller may write. */
  const char *requested = argv[1];
  bool writable = strncmp(requested, "writable-", 9U) == 0;
  const char *name = writable ? requested + 9U : requested;
  UmiStatus (*open_store)(const char *, UmiDataServer **) = writable
      ? umi_data_server_open_sqlite_existing : umi_data_server_open_sqlite_readonly;
  int result = 0;
  UmiDataServer *writer = NULL, *reader = NULL;
  char large[1025];
  memset(large, 'x', sizeof(large));
  large[1024] = 0;
  if (strcmp(name, "memory-mode") == 0) {
    OK(umi_data_server_create_memory(&writer));
    CHECK(!umi_data_server_is_readonly(writer));
    CHECK(!umi_data_server_is_readonly(NULL));
    goto cleanup;
  }
  if (strcmp(name, "null-output") == 0) {
    CHECK(open_store("store.sqlite", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  const char *invalid = NULL;
  bool invalid_case = true;
  if (strcmp(name, "empty") == 0)
    invalid = "";
  else if (strcmp(name, "special") == 0)
    invalid = ":memory:";
  else if (strcmp(name, "uri") == 0)
    invalid = "file:store.sqlite?mode=rwc";
  else if (strcmp(name, "long") == 0)
    invalid = large;
  else if (strcmp(name, "null") != 0)
    invalid_case = false;
  if (invalid_case) {
    reader = (UmiDataServer *)(void *)&result;
    UmiStatus status = open_store(invalid, &reader);
    /* A failed output-clearing assertion must not send a sentinel address to
     * the destructor. Preserve the check and make its failure path owned. */
    bool cleared = reader == NULL;
    if (reader == (UmiDataServer *)(void *)&result)
      reader = NULL;
    CHECK(cleared);
    CHECK(reader == NULL);
    CHECK(status ==
          (strcmp(name, "long") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT));
    goto cleanup;
  }
#ifndef UMICOM_HAS_SQLITE
  CHECK(strcmp(name, "unavailable") == 0);
  CHECK(open_store("store.sqlite", &reader) == UMI_STATUS_UNAVAILABLE);
  CHECK(reader == NULL);
  goto cleanup;
#else
  /* Only SQLite-enabled builds need a physical fixture filename. */
  char file[1024] = {0};
  CHECK(strcmp(name, "unavailable") != 0);
  CHECK(ro_path(argv[2], requested, file, true));
  if (strcmp(name, "missing") == 0) {
    size_t n = strlen(file);
    CHECK(n + 8 < sizeof(file));
    memcpy(file + n, ".missing", 9);
    CHECK(open_store(file, &reader) != UMI_STATUS_OK);
    CHECK(reader == NULL);
    FILE *probe = fopen(file, "rb");
    bool absent = probe == NULL;
    if (probe)
      fclose(probe);
    CHECK(absent);
    /* The original condition remains useful after safely closing an unexpected
     * file: the inspection opener must never have created it. */
    CHECK(probe == NULL);
    goto cleanup;
  }
  if (strcmp(name, "corrupt") == 0) {
    FILE *bad = fopen(file, "wb");
    CHECK(bad);
    size_t written = fwrite("not a database", 1, 14, bad);
    int closed = fclose(bad);
    CHECK(written == 14 && closed == 0);
    CHECK(open_store(file, &reader) != UMI_STATUS_OK);
    CHECK(reader == NULL);
    goto cleanup;
  }
  OK(umi_data_server_create_sqlite(file, &writer));
  CHECK(!umi_data_server_is_readonly(writer));
  OK(umi_data_server_set(writer, "original", "kept"));
  if (strcmp(name, "unrelated") == 0)
    OK(umi_data_server_execute(writer, "DROP TABLE umicom_kv; CREATE TABLE other(value TEXT);"));
  if (strcmp(name, "view") == 0)
    OK(umi_data_server_execute(
        writer, "DROP TABLE umicom_kv; CREATE VIEW umicom_kv AS SELECT 'k' AS key,'v' AS value;"));
  if (strcmp(name, "columns") == 0)
    OK(umi_data_server_execute(writer,
                               "DROP TABLE umicom_kv; CREATE TABLE umicom_kv(other TEXT);"));
  umi_data_server_destroy(writer);
  writer = NULL;
  UmiStatus status = open_store(file, &reader);
  if (strcmp(name, "valid") == 0) {
    CHECK(status == UMI_STATUS_OK && (umi_data_server_is_readonly(reader) != 0) == !writable);
    char value[32];
    OK(umi_data_server_get(reader, "original", value, sizeof(value)));
    CHECK(strcmp(value, "kept") == 0);
    UmiDataServerSnapshot snapshot;
    OK(umi_data_server_snapshot(reader, &snapshot));
    CHECK(snapshot.record_count == 1 && strcmp(snapshot.path, file) == 0);
    if (writable) {
      /* A new transaction must persist through close and an independent reader. */
      OK(umi_data_server_begin(reader));
      OK(umi_data_server_set(reader, "new", "persisted"));
      OK(umi_data_server_commit(reader));
      umi_data_server_destroy(reader);
      reader = NULL;
      OK(umi_data_server_open_sqlite_readonly(file, &reader));
      OK(umi_data_server_get(reader, "new", value, sizeof(value)));
      CHECK(strcmp(value, "persisted") == 0);
    }
  } else {
    CHECK(status != UMI_STATUS_OK && reader == NULL);
    /* A refused writable open must leave the original incompatible schema in
     * place. A second strict inspection must still see the admission failure. */
    if (writable) {
      CHECK(umi_data_server_open_sqlite_readonly(file, &reader) != UMI_STATUS_OK);
      CHECK(reader == NULL);
    }
  }
#endif
cleanup:
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  return result;
}
