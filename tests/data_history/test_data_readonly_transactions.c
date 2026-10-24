/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reader transactions compose with record services without writer reservations. */
#include "readonly_fixture.h"
int main(int argc, char **argv) {
  const char *cases[] = {"commit", "rollback", "nested",  "writer-reserved",     "reader-first",
                         "visit",  "snapshot", "outside", "denial-retains-owner"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiDataServer *writer = NULL, *reader = NULL;
  char file[1024], value[32];
  CHECK(ro_path(argv[2], name, file, true));
  OK(umi_data_server_create_sqlite(file, &writer));
  OK(umi_data_server_set(writer, "original", "kept"));
  OK(umi_data_server_open_sqlite_readonly(file, &reader));
  if (strcmp(name, "outside") == 0) {
    CHECK(umi_data_server_commit(reader) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_data_server_rollback(reader) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "writer-reserved") == 0) {
    OK(umi_data_server_begin(writer));
    OK(umi_data_server_set(writer, "original", "pending"));
  }
  OK(umi_data_server_begin(reader));
  CHECK(umi_data_server_in_transaction(reader));
  if (strcmp(name, "nested") == 0)
    CHECK(umi_data_server_begin(reader) == UMI_STATUS_BUSY);
  if (strcmp(name, "reader-first") == 0)
    OK(umi_data_server_begin(writer));
  OK(umi_data_server_get(reader, "original", value, sizeof(value)));
  CHECK(strcmp(value, "kept") == 0);
  if (strcmp(name, "visit") == 0) {
    size_t seen = 0;
    OK(umi_data_server_visit(reader, ro_count, &seen));
    CHECK(seen == 1);
  }
  if (strcmp(name, "snapshot") == 0) {
    UmiDataServerSnapshot snap;
    OK(umi_data_server_snapshot(reader, &snap));
    CHECK(snap.transaction_active && snap.record_count == 1);
  }
  if (strcmp(name, "denial-retains-owner") == 0) {
    CHECK(umi_data_server_delete(reader, "original") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_data_server_in_transaction(reader));
  }
  if (strcmp(name, "rollback") == 0)
    OK(umi_data_server_rollback(reader));
  else
    OK(umi_data_server_commit(reader));
  CHECK(!umi_data_server_in_transaction(reader));
  if (umi_data_server_in_transaction(writer))
    OK(umi_data_server_commit(writer));
  if (strcmp(name, "writer-reserved") == 0) {
    OK(umi_data_server_get(reader, "original", value, sizeof(value)));
    CHECK(strcmp(value, "pending") == 0);
  }
cleanup:
  if (reader && umi_data_server_in_transaction(reader))
    (void)umi_data_server_rollback(reader);
  if (writer && umi_data_server_in_transaction(writer))
    (void)umi_data_server_rollback(writer);
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  return result;
}
