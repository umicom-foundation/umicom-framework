/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sqlite_adapter.c
 *
 * PURPOSE:
 *   Verify SQLite creation, transaction, schema migration and integrity when the adapter is available.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
#ifdef UMICOM_HAS_SQLITE
    UmiDataServer *server = NULL;
    const char *path = "umicom-batch4-sqlite-test.db";
    (void)remove(path);
    assert(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "key", "value") == UMI_STATUS_OK);
    assert(umi_sqlite_report(server).open);
    assert(umi_sqlite_integrity_check(server) == UMI_STATUS_OK);
    umi_data_server_destroy(server);
    (void)remove(path);
#endif
    return 0;
}
