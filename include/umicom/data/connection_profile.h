/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_DATA_CONNECTION_PROFILE_H
#define UMICOM_DATA_CONNECTION_PROFILE_H
#include "umicom/data/explorer_types.h"
typedef struct UmiDatabaseConnectionProfile { char id[UMI_DATABASE_ID_CAPACITY]; char display_name[UMI_DATABASE_NAME_CAPACITY]; UmiDatabaseProvider provider; char endpoint[UMI_DATABASE_PATH_CAPACITY]; char database_name[UMI_DATABASE_NAME_CAPACITY]; char secret_reference[UMI_DATABASE_ID_CAPACITY]; uint32_t timeout_ms; int read_only; int require_tls; UmiDatabaseConnectionState state; char last_error[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseConnectionProfile;
typedef struct UmiDatabaseConnectionRegistry { UmiDatabaseConnectionProfile items[UMI_DATABASE_MAX_CONNECTIONS]; size_t count; char active_id[UMI_DATABASE_ID_CAPACITY]; uint64_t revision; } UmiDatabaseConnectionRegistry;
UmiStatus umi_database_connection_registry_init(UmiDatabaseConnectionRegistry *registry);
UmiStatus umi_database_connection_registry_upsert(UmiDatabaseConnectionRegistry *registry,const UmiDatabaseConnectionProfile *profile);
UmiStatus umi_database_connection_registry_activate(UmiDatabaseConnectionRegistry *registry,const char *id);
UmiStatus umi_database_connection_registry_active(const UmiDatabaseConnectionRegistry *registry,UmiDatabaseConnectionProfile *out_profile);
#endif
