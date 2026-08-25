/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_history.h
 *
 * PURPOSE:
 *   Maintain bounded migration completion evidence for audit and startup compatibility checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_HISTORY_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_HISTORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataMigrationHistoryEntry { char migration_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; uint64_t applied_at; uint64_t schema_fingerprint; bool success; } UmiDataMigrationHistoryEntry;
typedef struct UmiDataMigrationHistory { UmiDataMigrationHistoryEntry items[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiDataMigrationHistory;
/* Reset migration history evidence. */ void umi_data_migration_history_init(UmiDataMigrationHistory *history);
/* Append immutable migration outcome evidence. */ UmiStatus umi_data_migration_history_append(UmiDataMigrationHistory *history,const char *migration_id,uint64_t applied_at,uint64_t schema_fingerprint,bool success);
/* Find the most recent successful schema fingerprint. */ UmiStatus umi_data_migration_history_latest_success(const UmiDataMigrationHistory *history,uint64_t *out_fingerprint);

#ifdef __cplusplus
}
#endif
#endif
