/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/data_server.h
 *
 * PURPOSE:
 *   Define the authoritative key/value Data Server with memory and SQLite backends, explicit transactions, SQL execution, integrity information and stable ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_DATA_SERVER_H
#define UMICOM_DATA_DATA_SERVER_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataServer UmiDataServer;

typedef enum UmiDataServerBackend {
    UMI_DATA_BACKEND_MEMORY = 1,
    UMI_DATA_BACKEND_SQLITE = 2
} UmiDataServerBackend;

/* The visitor receives borrowed key/value strings valid only for the callback.
 * Returning a non-OK status stops enumeration and propagates that status. */
typedef UmiStatus (*UmiDataServerRecordVisitor)(const char *key,
                                                const char *value,
                                                void *user_data);

typedef struct UmiDataServerSnapshot {
    UmiDataServerBackend backend;
    size_t record_count;
    int transaction_active;
    char backend_name[32];
    char path[1024];
    char last_error[512];
} UmiDataServerSnapshot;

UmiStatus umi_data_server_create_memory(UmiDataServer **out_server);
UmiStatus umi_data_server_create_sqlite(const char *database_path,
                                        UmiDataServer **out_server);
void umi_data_server_destroy(UmiDataServer *server);
UmiStatus umi_data_server_set(UmiDataServer *server,
                              const char *key,
                              const char *value);
UmiStatus umi_data_server_get(const UmiDataServer *server,
                              const char *key,
                              char *value,
                              size_t value_capacity);
UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key);
size_t umi_data_server_count(const UmiDataServer *server);
UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server);
const char *umi_data_server_backend_name(const UmiDataServer *server);

UmiStatus umi_data_server_begin(UmiDataServer *server);
UmiStatus umi_data_server_commit(UmiDataServer *server);
UmiStatus umi_data_server_rollback(UmiDataServer *server);
int umi_data_server_in_transaction(const UmiDataServer *server);
UmiStatus umi_data_server_execute(UmiDataServer *server, const char *sql);
const char *umi_data_server_path(const UmiDataServer *server);
const char *umi_data_server_last_error(const UmiDataServer *server);
UmiStatus umi_data_server_visit(const UmiDataServer *server,
                                UmiDataServerRecordVisitor visitor,
                                void *user_data);
UmiStatus umi_data_server_snapshot(const UmiDataServer *server,
                                   UmiDataServerSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
