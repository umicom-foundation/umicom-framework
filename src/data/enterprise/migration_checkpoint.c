/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_checkpoint.c
 *
 * PURPOSE:
 *   Record resumable migration position and pre/post schema fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/migration_checkpoint.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_migration_checkpoint_init(UmiDataMigrationCheckpoint *item, const char *checkpoint_id, const char *migration_id, size_t completed_steps, uint64_t source_fingerprint, uint64_t current_fingerprint) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->checkpoint_id,sizeof(item->checkpoint_id),checkpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->migration_id,sizeof(item->migration_id),migration_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->completed_steps=completed_steps;item->source_fingerprint=source_fingerprint;item->current_fingerprint=current_fingerprint;item->committed=false;
    return umi_data_migration_checkpoint_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_migration_checkpoint_init_build_value(UmiDataMigrationCheckpoint *item, const char *checkpoint_id, const char *migration_id, size_t completed_steps, uint64_t source_fingerprint, uint64_t current_fingerprint) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->checkpoint_id,sizeof(item->checkpoint_id),checkpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->migration_id,sizeof(item->migration_id),migration_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->completed_steps=completed_steps;item->source_fingerprint=source_fingerprint;item->current_fingerprint=current_fingerprint;item->committed=false;
    return umi_data_migration_checkpoint_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_migration_checkpoint_init, UmiDataMigrationCheckpoint, item,
    (UmiDataMigrationCheckpoint *item, const char *checkpoint_id, const char *migration_id, size_t completed_steps, uint64_t source_fingerprint, uint64_t current_fingerprint),
    umi_data_migration_checkpoint_init_build_value(candidate, checkpoint_id, migration_id, completed_steps, source_fingerprint, current_fingerprint))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_migration_checkpoint_validate(const UmiDataMigrationCheckpoint *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->checkpoint_id, '\0', sizeof(item->checkpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->migration_id, '\0', sizeof(item->migration_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->checkpoint_id[0] != '\0' && item->migration_id[0] != '\0' && item->source_fingerprint != 0U && item->current_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataMigrationCheckpointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd273fa3f52630d8a);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataMigrationCheckpoint *)0)->checkpoint_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataMigrationCheckpoint *)0)->migration_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataMigrationCheckpointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataMigrationCheckpoint *)0)->checkpoint_id) - 1U +
        8U + sizeof(((UmiDataMigrationCheckpoint *)0)->migration_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataMigrationCheckpointArchiveWrite(UmiArchiveWriter *writer, const UmiDataMigrationCheckpoint *value)
{
    UmiArchiveWriteText(writer, value->checkpoint_id, sizeof(value->checkpoint_id));
    UmiArchiveWriteText(writer, value->migration_id, sizeof(value->migration_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->completed_steps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->current_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->committed);
}
static void UmiDataMigrationCheckpointArchiveRead(UmiArchiveReader *reader, UmiDataMigrationCheckpoint *value)
{
    UmiArchiveReadText(reader, value->checkpoint_id, sizeof(value->checkpoint_id));
    UmiArchiveReadText(reader, value->migration_id, sizeof(value->migration_id));
    value->completed_steps = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->source_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->current_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->committed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataMigrationCheckpointArchiveValidate(const UmiDataMigrationCheckpoint *value)
{
    return umi_data_migration_checkpoint_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_migration_checkpoint_archive_encode, umi_data_migration_checkpoint_archive_decode,
    UmiDataMigrationCheckpoint, UmiDataMigrationCheckpointArchiveSchema, UmiDataMigrationCheckpointArchiveBound, UmiDataMigrationCheckpointArchiveWrite, UmiDataMigrationCheckpointArchiveRead, UmiDataMigrationCheckpointArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_migration_checkpoint_archive_decode_if_unchanged, umi_data_migration_checkpoint_archive_encode,
    UmiDataMigrationCheckpoint, UmiDataMigrationCheckpointArchiveSchema, UmiDataMigrationCheckpointArchiveBound, UmiDataMigrationCheckpointArchiveRead, UmiDataMigrationCheckpointArchiveValidate)
