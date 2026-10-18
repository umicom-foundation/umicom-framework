/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_step.h
 *
 * PURPOSE:
 *   Describe one reversible or irreversible schema/data migration operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_STEP_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_STEP_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data migration step data shared with callers of this public contract.
 */
typedef struct UmiDataMigrationStep {
    char step_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char description[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    uint32_t ordinal;
    bool reversible;
    bool destructive;
} UmiDataMigrationStep;

/* Initialise a validated migration step descriptor. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_migration_step_init(UmiDataMigrationStep *item, const char *step_id, const char *description, uint32_t ordinal, bool reversible, bool destructive);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_migration_step_validate(const UmiDataMigrationStep *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_migration_step_archive_encode(const UmiDataMigrationStep *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_migration_step_archive_decode(const void *bytes, size_t byte_count,
    UmiDataMigrationStep *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDataMigrationStep object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_data_migration_step_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDataMigrationStep *workspace, UmiDataMigrationStep *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_migration_step_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataMigrationStep *expected, UmiDataMigrationStep *value);


#ifdef __cplusplus
}
#endif
#endif
