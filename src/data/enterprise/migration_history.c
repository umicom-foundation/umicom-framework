/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_history.c
 *
 * PURPOSE:
 *   Maintain bounded migration completion evidence for audit and startup compatibility checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_history.h"
#include <string.h>

/* History is bounded because durable persistence belongs to the canonical Data Server store. */ void umi_data_migration_history_init(UmiDataMigrationHistory *history){if(history!=NULL)(void)memset(history,0,sizeof(*history));}
/* Append refuses duplicate migration IDs to preserve once-only migration identity. */ UmiStatus umi_data_migration_history_append(UmiDataMigrationHistory *history,const char *migration_id,uint64_t applied_at,uint64_t schema_fingerprint,bool success){size_t i;UmiStatus s;if(history==NULL||migration_id==NULL||schema_fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<history->count;++i)if(strcmp(history->items[i].migration_id,migration_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(history->count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_data_enterprise_copy_text(history->items[history->count].migration_id,sizeof(history->items[history->count].migration_id),migration_id);if(s!=UMI_STATUS_OK)return s;history->items[history->count].applied_at=applied_at;history->items[history->count].schema_fingerprint=schema_fingerprint;history->items[history->count].success=success;history->count++;return UMI_STATUS_OK;}
/* Latest-success lookup supports startup validation and migration continuation. */ UmiStatus umi_data_migration_history_latest_success(const UmiDataMigrationHistory *history,uint64_t *out_fingerprint){size_t i;if(history==NULL||out_fingerprint==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=history->count;i>0U;--i)if(history->items[i-1U].success){*out_fingerprint=history->items[i-1U].schema_fingerprint;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
