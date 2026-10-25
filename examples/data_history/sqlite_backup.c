/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/sqlite_backup.c
 *
 * PURPOSE:
 *   Installed data-only consumer: copy, recover, inspect and retain both files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Installed data-only consumer: copy, recover, inspect and retain both files. */
#include "umicom/data/backup.h"
#include "umicom/data/sqlite.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#define REQUIRE(x)                                                                                 \
  do {                                                                                             \
    UmiStatus status = (x);                                                                        \
    if (status != UMI_STATUS_OK) {                                                                 \
      fprintf(stderr, "%s: %s\n", #x, umi_status_text(status));                                    \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Supply an existing absolute output directory.\n");
    return 2;
  }
  int result = 1;
  UmiDataServer *source = NULL, *copy = NULL;
  char backup[1024], recovered[1024];
  struct timespec now;
  if (timespec_get(&now, TIME_UTC) != TIME_UTC)
    return 1;
  int count = snprintf(backup, sizeof(backup), "%s/example-backup-%lld-%ld.sqlite", argv[1],
                       (long long)now.tv_sec, now.tv_nsec);
  if (count < 0 || (size_t)count >= sizeof(backup))
    return 1;
  count = snprintf(recovered, sizeof(recovered), "%s.recovered", backup);
  if (count < 0 || (size_t)count >= sizeof(recovered))
    return 1;
  REQUIRE(umi_data_server_create_sqlite(":memory:", &source));
  REQUIRE(umi_data_server_set(source, "fictional.parcel", "ready"));
  UmiDataBackupReport created, restored;
  REQUIRE(umi_data_backup_create_checked(source, backup, &created));
  REQUIRE(umi_data_backup_restore_checked(backup, recovered, &restored));
  REQUIRE(umi_data_server_open_sqlite_readonly(recovered, &copy));
  char value[32];
  REQUIRE(umi_data_server_get(copy, "fictional.parcel", value, sizeof(value)));
  if (strcmp(value, "ready") != 0 || created.record_count != 1 || restored.record_count != 1)
    goto cleanup;
  UmiSqliteHealthReport health;
  REQUIRE(umi_sqlite_health_read(copy, &health));
  if (!health.sound)
    goto cleanup;
  printf("Backup retained at %s\nRecovered store retained at %s\n", backup, recovered);
  result = 0;
cleanup:
  umi_data_server_destroy(copy);
  umi_data_server_destroy(source);
  return result;
}
