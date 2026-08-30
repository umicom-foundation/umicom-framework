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
typedef struct UmiDatabaseQueryRecord { uint64_t sequence; char statement[UMI_DATABASE_SQL_CAPACITY]; UmiDatabaseQueryKind kind; UmiStatus status; size_t affected_rows; uint64_t duration_us; char message[UMI_DATABASE_TEXT_CAPACITY]; } UmiDatabaseQueryRecord;
typedef struct UmiDatabaseQueryHistory { UmiDatabaseQueryRecord items[UMI_DATABASE_MAX_HISTORY]; size_t count; uint64_t next_sequence; } UmiDatabaseQueryHistory;
UmiStatus umi_database_query_history_init(UmiDatabaseQueryHistory *history);
UmiStatus umi_database_query_history_append(UmiDatabaseQueryHistory *history,const UmiDatabaseQueryRecord *record);
UmiStatus umi_database_query_history_at(const UmiDatabaseQueryHistory *history,size_t index,UmiDatabaseQueryRecord *out_record);
void umi_database_query_history_clear(UmiDatabaseQueryHistory *history);
#endif
