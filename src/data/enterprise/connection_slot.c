/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/connection_slot.c
 *
 * PURPOSE:
 *   Describe one pooled backend connection lease slot without owning the backend handle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/connection_slot.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_connection_slot_init(UmiDataConnectionSlot *item, const char *slot_id, uint64_t connection_token) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->slot_id,sizeof(item->slot_id),slot_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->connection_token=connection_token;item->healthy=true;item->leased=false;
    return umi_data_connection_slot_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_connection_slot_init_build_value(UmiDataConnectionSlot *item, const char *slot_id, uint64_t connection_token) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->slot_id,sizeof(item->slot_id),slot_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->connection_token=connection_token;item->healthy=true;item->leased=false;
    return umi_data_connection_slot_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_connection_slot_init, UmiDataConnectionSlot, item,
    (UmiDataConnectionSlot *item, const char *slot_id, uint64_t connection_token),
    umi_data_connection_slot_init_build_value(candidate, slot_id, connection_token))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_connection_slot_validate(const UmiDataConnectionSlot *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->slot_id, '\0', sizeof(item->slot_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->slot_id[0] != '\0' && item->connection_token != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataConnectionSlotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd6d0c3d3617292b3);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataConnectionSlot *)0)->slot_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataConnectionSlotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataConnectionSlot *)0)->slot_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataConnectionSlotArchiveWrite(UmiArchiveWriter *writer, const UmiDataConnectionSlot *value)
{
    UmiArchiveWriteText(writer, value->slot_id, sizeof(value->slot_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->connection_token);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_used_at);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->lease_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->healthy);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->leased);
}
static void UmiDataConnectionSlotArchiveRead(UmiArchiveReader *reader, UmiDataConnectionSlot *value)
{
    UmiArchiveReadText(reader, value->slot_id, sizeof(value->slot_id));
    value->connection_token = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_used_at = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->lease_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->healthy = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->leased = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataConnectionSlotArchiveValidate(const UmiDataConnectionSlot *value)
{
    return umi_data_connection_slot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_connection_slot_archive_encode, umi_data_connection_slot_archive_decode,
    UmiDataConnectionSlot, UmiDataConnectionSlotArchiveSchema, UmiDataConnectionSlotArchiveBound, UmiDataConnectionSlotArchiveWrite, UmiDataConnectionSlotArchiveRead, UmiDataConnectionSlotArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_data_connection_slot_archive_decode_with_workspace, UmiDataConnectionSlot,
    UmiDataConnectionSlotArchiveSchema, UmiDataConnectionSlotArchiveBound, UmiDataConnectionSlotArchiveRead, UmiDataConnectionSlotArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_connection_slot_archive_decode_if_unchanged, umi_data_connection_slot_archive_encode,
    UmiDataConnectionSlot, UmiDataConnectionSlotArchiveSchema, UmiDataConnectionSlotArchiveBound, UmiDataConnectionSlotArchiveRead, UmiDataConnectionSlotArchiveValidate)
