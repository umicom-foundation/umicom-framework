/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/data_server.h
 *
 * PURPOSE:
 *   Define the authoritative key/value Data Server with memory and SQLite backends, explicit transactions, SQL execution, integrity information and stable ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_DATA_SERVER_H
#define UMICOM_DATA_DATA_SERVER_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data server data shared with callers of this public contract.
 */
typedef struct UmiDataServer UmiDataServer;

/**
 * List the named data server backend values accepted by this public contract.
 */
typedef enum UmiDataServerBackend {
    UMI_DATA_BACKEND_MEMORY = 1,
    UMI_DATA_BACKEND_SQLITE = 2
} UmiDataServerBackend;

/* The visitor receives borrowed key/value strings valid only for the callback.
 * Returning a non-OK status stops enumeration and propagates that status. */
typedef UmiStatus (*UmiDataServerRecordVisitor)(const char *key,
                                                const char *value,
                                                void *user_data);

/**
 * Represent the data server snapshot data shared with callers of this public contract.
 */
typedef struct UmiDataServerSnapshot {
    UmiDataServerBackend backend;
    size_t record_count;
    int transaction_active;
    char backend_name[32];
    char path[1024];
    char last_error[512];
} UmiDataServerSnapshot;

/**
 * Provide the data server create memory operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_create_memory(UmiDataServer **out_server);
/**
 * Provide the data server create sqlite operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_create_sqlite(const char *database_path,
                                        UmiDataServer **out_server);
/**
 * Release or reset state held by data server so the same storage can be reused safely.
 */
void umi_data_server_destroy(UmiDataServer *server);
/**
 * Copy data server into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_data_server_set(UmiDataServer *server,
                              const char *key,
                              const char *value);
/**
 * Provide the data server get operation used by this module and its client applications.
 */
UmiStatus umi_data_server_get(const UmiDataServer *server,
                              const char *key,
                              char *value,
                              size_t value_capacity);
/**
 * Provide the data server delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key);
/**
 * Return the number of records represented by data server without changing their state.
 */
size_t umi_data_server_count(const UmiDataServer *server);
/**
 * Provide the data server backend operation used by this module and its client
 * applications.
 */
UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server);
/**
 * Provide the data server backend name operation used by this module and its client
 * applications.
 */
const char *umi_data_server_backend_name(const UmiDataServer *server);

/**
 * Provide the data server begin operation used by this module and its client applications.
 */
UmiStatus umi_data_server_begin(UmiDataServer *server);
/**
 * Provide the data server commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_commit(UmiDataServer *server);
/**
 * Provide the data server rollback operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_rollback(UmiDataServer *server);
/**
 * Provide the data server in transaction operation used by this module and its client
 * applications.
 */
int umi_data_server_in_transaction(const UmiDataServer *server);
/**
 * Perform data server through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_data_server_execute(UmiDataServer *server, const char *sql);
/**
 * Provide the data server path operation used by this module and its client applications.
 */
const char *umi_data_server_path(const UmiDataServer *server);
/**
 * Provide the data server last error operation used by this module and its client
 * applications.
 */
const char *umi_data_server_last_error(const UmiDataServer *server);
/**
 * Provide the data server visit operation used by this module and its client applications.
 */
UmiStatus umi_data_server_visit(const UmiDataServer *server,
                                UmiDataServerRecordVisitor visitor,
                                void *user_data);
/**
 * Provide the data server snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_snapshot(const UmiDataServer *server,
                                   UmiDataServerSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
