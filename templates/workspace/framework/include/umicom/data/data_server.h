/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/data/data_server.h
 *
 * PURPOSE:
 *   Declare the data server contract shared by Framework services and thin
 *   applications.
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
UmiStatus umi_data_server_set(UmiDataServer *server, const char *key,
                              const char *value);
/**
 * Provide the data server get operation used by this module and its client applications.
 */
UmiStatus umi_data_server_get(const UmiDataServer *server, const char *key,
                              char *value, size_t value_capacity);
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

#ifdef __cplusplus
}
#endif

#endif
