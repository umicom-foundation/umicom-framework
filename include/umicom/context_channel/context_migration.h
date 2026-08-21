/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_migration.h
 *
 * PURPOSE:
 *   Record schema migration plans for persisted context records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_MIGRATION_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_MIGRATION_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextMigration {
    uint32_t structure_size;
    char migration_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char from_version[UMI_CONTEXT_VALUE_CAPACITY];
    char to_version[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextMigration;
void umi_context_migration_init(UmiContextMigration *state);
UmiStatus umi_context_migration_set_field(UmiContextMigration *state,size_t field_index,const char *value);
const char *umi_context_migration_field(const UmiContextMigration *state,size_t field_index);
UmiStatus umi_context_migration_record_success(UmiContextMigration *state,uint64_t sequence);
UmiStatus umi_context_migration_record_failure(UmiContextMigration *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_migration_validate(const UmiContextMigration *state);
bool umi_context_migration_covers_sequence(const UmiContextMigration *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
