/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_foreign_key.h
 *
 * PURPOSE:
 *   Describe referential constraints in a backend-neutral form for migration ordering and ORM relations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_FOREIGN_KEY_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_FOREIGN_KEY_H

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
 * Represent the data schema foreign key data shared with callers of this public contract.
 */
typedef struct UmiDataSchemaForeignKey {
    char constraint_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char target_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char source_column[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char target_column[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    bool cascade_delete;
} UmiDataSchemaForeignKey;

/* Initialise a validated schema foreign key descriptor. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_schema_foreign_key_init(UmiDataSchemaForeignKey *item, const char *constraint_id, const char *source_table, const char *source_column, const char *target_table, const char *target_column);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_schema_foreign_key_validate(const UmiDataSchemaForeignKey *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_schema_foreign_key_archive_encode(const UmiDataSchemaForeignKey *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_schema_foreign_key_archive_decode(const void *bytes, size_t byte_count,
    UmiDataSchemaForeignKey *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_schema_foreign_key_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataSchemaForeignKey *expected, UmiDataSchemaForeignKey *value);


#ifdef __cplusplus
}
#endif
#endif
