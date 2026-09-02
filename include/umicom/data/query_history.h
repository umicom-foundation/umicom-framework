/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/query_history.h
 *
 * PURPOSE:
 *   Publish the public query history contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_QUERY_HISTORY_H
#define UMICOM_DATA_QUERY_HISTORY_H
#include "umicom/data/explorer_types.h"
/**
 * Represent the database query record data shared with callers of this public contract.
 */
typedef struct UmiDatabaseQueryRecord { uint64_t sequence; char statement[UMI_DATABASE_SQL_CAPACITY]; UmiDatabaseQueryKind kind; UmiStatus status; size_t affected_rows; uint64_t duration_us; char message[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseQueryRecord;
/**
 * Represent the database query history data shared with callers of this public contract.
 */
typedef struct UmiDatabaseQueryHistory { UmiDatabaseQueryRecord items[UMI_DATABASE_MAX_HISTORY]; size_t count; uint64_t next_sequence; } UmiDatabaseQueryHistory;
/**
 * Initialise database query history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_query_history_init(UmiDatabaseQueryHistory *history);
/**
 * Add database query history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_database_query_history_append(UmiDatabaseQueryHistory *history,const UmiDatabaseQueryRecord *record);
/**
 * Find database query history while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_database_query_history_at(const UmiDatabaseQueryHistory *history,size_t index,UmiDatabaseQueryRecord *out_record);
/**
 * Release or reset state held by database query history so the same storage can be reused
 * safely.
 */
void umi_database_query_history_clear(UmiDatabaseQueryHistory *history);
#endif
