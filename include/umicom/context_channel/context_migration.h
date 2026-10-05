/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_migration.h
 *
 * PURPOSE:
 *   Record schema migration plans for persisted context records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_MIGRATION_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_MIGRATION_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context migration data shared with callers of this public contract.
 */
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
/**
 * Initialise context migration from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_migration_init(UmiContextMigration *state);
/**
 * Provide the context migration set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_migration_set_field(UmiContextMigration *state,size_t field_index,const char *value);
/**
 * Provide the context migration field operation used by this module and its client
 * applications.
 */
const char *umi_context_migration_field(const UmiContextMigration *state,size_t field_index);
/**
 * Provide the context migration record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_migration_record_success(UmiContextMigration *state,uint64_t sequence);
/**
 * Provide the context migration record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_migration_record_failure(UmiContextMigration *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context migration satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_migration_validate(const UmiContextMigration *state);
/**
 * Provide the context migration covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_migration_covers_sequence(const UmiContextMigration *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_migration_archive_encode(const UmiContextMigration *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_migration_archive_decode(const void *bytes, size_t byte_count,
    UmiContextMigration *value);

#ifdef __cplusplus
}
#endif
#endif
