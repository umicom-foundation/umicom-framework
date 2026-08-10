/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/integrity.c
 *
 * PURPOSE:
 *   Implement portable Data Server integrity inspection and SQLite-specific quick checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/integrity.h"

#include <string.h>

#include "umicom/data/schema_version.h"
#include "umicom/data/sqlite.h"
#include "umicom/data/store.h"

UmiStatus umi_data_integrity_check(UmiDataServer *server,
                                   UmiDataIntegrityReport *out_report)
{
    UmiStore store;
    UmiStatus status;
    if (server == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->backend = umi_data_server_backend(server);
    out_report->backend_name = umi_data_server_backend_name(server);
    out_report->path = umi_data_server_path(server);
    out_report->records = umi_data_server_count(server);
    out_report->transaction_active = umi_data_server_in_transaction(server);
    status = umi_store_from_data_server(server, &store);
    if (status == UMI_STATUS_OK) {
        status = umi_schema_version_get(&store, &out_report->schema_version);
    }
    if (status == UMI_STATUS_OK &&
        out_report->backend == UMI_DATA_BACKEND_SQLITE) {
        status = umi_sqlite_integrity_check(server);
    }
    out_report->integrity_status = status;
    return status;
}
