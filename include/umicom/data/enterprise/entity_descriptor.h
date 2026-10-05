/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/entity_descriptor.h
 *
 * PURPOSE:
 *   Describe an ORM entity without coupling persistence mapping to application structs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_ENTITY_DESCRIPTOR_H
#define UMICOM_DATA_ENTERPRISE_ENTITY_DESCRIPTOR_H

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
 * Represent the data entity descriptor data shared with callers of this public contract.
 */
typedef struct UmiDataEntityDescriptor {
    char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char table_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char identity_field[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    size_t field_count;
    bool immutable;
} UmiDataEntityDescriptor;

/* Initialise a validated entity descriptor descriptor. */
UmiStatus umi_data_entity_descriptor_init(UmiDataEntityDescriptor *item, const char *entity_id, const char *table_id, const char *identity_field);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_entity_descriptor_validate(const UmiDataEntityDescriptor *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_entity_descriptor_archive_encode(const UmiDataEntityDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_entity_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiDataEntityDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
