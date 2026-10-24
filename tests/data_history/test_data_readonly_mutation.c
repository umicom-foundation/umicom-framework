/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public mutation routes cannot turn an inspection connection into a writer. */
#include "readonly_fixture.h"
int main(int argc, char **argv) {
  const char *cases[] = {"set",    "delete",      "insert",     "drop",
                         "attach", "pragma",      "vacuum",     "select",
                         "multi",  "transaction", "diagnostic", "read-after-denial"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  int result = 0;
  UmiDataServer *writer = NULL, *reader = NULL;
  char file[1024];
  const char *name = argv[1];
  CHECK(ro_path(argv[2], name, file, true));
  OK(umi_data_server_create_sqlite(file, &writer));
  OK(umi_data_server_set(writer, "original", "kept"));
  OK(umi_data_server_open_sqlite_readonly(file, &reader));
  const char *sql = "SELECT value FROM umicom_kv;";
  if (strcmp(name, "insert") == 0)
    sql = "INSERT INTO umicom_kv VALUES('new','bad');";
  if (strcmp(name, "drop") == 0)
    sql = "DROP TABLE umicom_kv;";
  if (strcmp(name, "attach") == 0)
    sql = "ATTACH ':memory:' AS extra;";
  if (strcmp(name, "pragma") == 0)
    sql = "PRAGMA query_only=OFF;";
  if (strcmp(name, "vacuum") == 0)
    sql = "VACUUM;";
  if (strcmp(name, "multi") == 0)
    sql = "SELECT 1; UPDATE umicom_kv SET value='bad';";
  if (strcmp(name, "transaction") == 0)
    OK(umi_data_server_begin(reader));
  UmiStatus denied;
  if (strcmp(name, "set") == 0)
    denied = umi_data_server_set(reader, "original", "bad");
  else if (strcmp(name, "delete") == 0)
    denied = umi_data_server_delete(reader, "original");
  else
    denied = umi_data_server_execute(reader, sql);
  CHECK(denied == UMI_STATUS_PERMISSION_DENIED);
  char error[512];
  OK(UmiDataServerCopyError(reader, error, sizeof(error)));
  CHECK(strstr(error, "inspection") != NULL);
  if (strcmp(name, "transaction") == 0) {
    CHECK(umi_data_server_in_transaction(reader));
    OK(umi_data_server_commit(reader));
  }
  char value[32];
  OK(umi_data_server_get(reader, "original", value, sizeof(value)));
  CHECK(strcmp(value, "kept") == 0);
  size_t count = 0;
  OK(UmiDataServerCountChecked(reader, &count));
  CHECK(count == 1);
  if (strcmp(name, "read-after-denial") == 0) {
    OK(umi_data_server_begin(reader));
    OK(umi_data_server_get(reader, "original", value, sizeof(value)));
    OK(umi_data_server_rollback(reader));
  }
  OK(umi_data_server_set(writer, "writable", "still supported"));
  OK(umi_data_server_get(reader, "writable", value, sizeof(value)));
  CHECK(strcmp(value, "still supported") == 0);
cleanup:
  umi_data_server_destroy(reader);
  umi_data_server_destroy(writer);
  return result;
}
