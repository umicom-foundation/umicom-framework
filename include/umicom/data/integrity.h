/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/integrity.h
 *
 * PURPOSE:
 *   Define a portable Data Server integrity report covering backend, path, records, schema version, transaction state and SQLite health.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_INTEGRITY_H
#define UMICOM_DATA_INTEGRITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataIntegrityReport {
    UmiDataServerBackend backend;
    const char *backend_name;
    const char *path;
    size_t records;
    uint32_t schema_version;
    int transaction_active;
    UmiStatus integrity_status;
} UmiDataIntegrityReport;

UmiStatus umi_data_integrity_check(UmiDataServer *server,
                                   UmiDataIntegrityReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
