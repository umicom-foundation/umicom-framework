/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_position.c
 *
 * PURPOSE:
 *   Implement represent settled, pending-in and pending-out custody quantities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_position.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury custody position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_position_init(UmiTreasuryCustodyPosition *value,
    const char *id,
    int64_t settled_quantity,
    int64_t pending_in_quantity,
    int64_t pending_out_quantity) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->settled_quantity=settled_quantity;
    value->pending_in_quantity=pending_in_quantity;
    value->pending_out_quantity=pending_out_quantity;
    return umi_treasury_custody_position_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury custody position satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_custody_position_valid(const UmiTreasuryCustodyPosition *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->settled_quantity >= 0 && value->pending_in_quantity >= 0 && value->pending_out_quantity >= 0);
}

/*
 * Provide the treasury custody position projected quantity operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_custody_position_projected_quantity(const UmiTreasuryCustodyPosition *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->settled_quantity + value->pending_in_quantity - value->pending_out_quantity;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCustodyPositionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xea1c83fc5ad5dd85);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCustodyPosition *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCustodyPositionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCustodyPosition *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryCustodyPositionArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCustodyPosition *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->settled_quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->pending_in_quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->pending_out_quantity);
}
static void UmiTreasuryCustodyPositionArchiveRead(UmiArchiveReader *reader, UmiTreasuryCustodyPosition *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->settled_quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->pending_in_quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->pending_out_quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCustodyPositionArchiveValidate(const UmiTreasuryCustodyPosition *value)
{
    return umi_treasury_custody_position_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_custody_position_archive_encode, umi_treasury_custody_position_archive_decode,
    UmiTreasuryCustodyPosition, UmiTreasuryCustodyPositionArchiveSchema, UmiTreasuryCustodyPositionArchiveBound, UmiTreasuryCustodyPositionArchiveWrite, UmiTreasuryCustodyPositionArchiveRead, UmiTreasuryCustodyPositionArchiveValidate)
