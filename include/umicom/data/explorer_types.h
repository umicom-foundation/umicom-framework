/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/explorer_types.h
 *
 * PURPOSE:
 *   Publish the public explorer types contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Database credentials never live in these snapshots.  A
 * secret_reference names a credential held by the security boundary. */
#ifndef UMICOM_DATA_EXPLORER_TYPES_H
#define UMICOM_DATA_EXPLORER_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_DATABASE_ID_CAPACITY 128U
#define UMI_DATABASE_NAME_CAPACITY 256U
#define UMI_DATABASE_PATH_CAPACITY 1024U
#define UMI_DATABASE_TEXT_CAPACITY 4096U
#define UMI_DATABASE_SQL_CAPACITY 16384U
#define UMI_DATABASE_RESULT_CAPACITY 65536U
#define UMI_DATABASE_MAX_CONNECTIONS 32U
#define UMI_DATABASE_MAX_TABLES 128U
#define UMI_DATABASE_MAX_COLUMNS 64U
#define UMI_DATABASE_MAX_HISTORY 128U
#define UMI_DATABASE_MAX_MIGRATIONS 128U
#define UMI_DATABASE_MAX_TRANSFERS 64U
#define UMI_DATABASE_MAX_BACKUPS 64U
/**
 * List the named database provider values accepted by this public contract.
 */
typedef enum UmiDatabaseProvider { UMI_DATABASE_PROVIDER_MEMORY=1, UMI_DATABASE_PROVIDER_SQLITE=2, UMI_DATABASE_PROVIDER_POSTGRESQL=3, UMI_DATABASE_PROVIDER_ORACLE=4, UMI_DATABASE_PROVIDER_MYSQL=5, UMI_DATABASE_PROVIDER_MONGODB=6, UMI_DATABASE_PROVIDER_CUSTOM=7 } UmiDatabaseProvider;
/**
 * List the named database connection state values accepted by this public contract.
 */
typedef enum UmiDatabaseConnectionState { UMI_DATABASE_DISCONNECTED=0, UMI_DATABASE_CONNECTING=1, UMI_DATABASE_READY=2, UMI_DATABASE_FAILED=3 } UmiDatabaseConnectionState;
/**
 * List the named database query kind values accepted by this public contract.
 */
typedef enum UmiDatabaseQueryKind { UMI_DATABASE_QUERY_UNKNOWN=0, UMI_DATABASE_QUERY_READ=1, UMI_DATABASE_QUERY_WRITE=2, UMI_DATABASE_QUERY_SCHEMA=3, UMI_DATABASE_QUERY_TRANSACTION=4 } UmiDatabaseQueryKind;
/**
 * List the named database transfer direction values accepted by this public contract.
 */
typedef enum UmiDatabaseTransferDirection { UMI_DATABASE_TRANSFER_IMPORT=1, UMI_DATABASE_TRANSFER_EXPORT=2 } UmiDatabaseTransferDirection;
/**
 * List the named database transfer format values accepted by this public contract.
 */
typedef enum UmiDatabaseTransferFormat { UMI_DATABASE_FORMAT_CSV=1, UMI_DATABASE_FORMAT_JSON=2, UMI_DATABASE_FORMAT_SQL=3 } UmiDatabaseTransferFormat;
/**
 * Provide the database copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_copy_text(char *destination,size_t capacity,const char *source);
/**
 * Provide the database provider text operation used by this module and its client
 * applications.
 */
const char *umi_database_provider_text(UmiDatabaseProvider provider);
/**
 * Provide the database query kind text operation used by this module and its client
 * applications.
 */
const char *umi_database_query_kind_text(UmiDatabaseQueryKind kind);
#endif
