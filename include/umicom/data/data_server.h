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

UmiStatus umi_data_server_create_memory(UmiDataServer **out_server);
UmiStatus umi_data_server_create_sqlite(const char *database_path,
                                        UmiDataServer **out_server);
void umi_data_server_destroy(UmiDataServer *server);
UmiStatus umi_data_server_set(UmiDataServer *server, const char *key,
                              const char *value);
UmiStatus umi_data_server_get(const UmiDataServer *server, const char *key,
                              char *value, size_t value_capacity);
UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key);
size_t umi_data_server_count(const UmiDataServer *server);
UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server);
const char *umi_data_server_backend_name(const UmiDataServer *server);

#ifdef __cplusplus
}
#endif

#endif
