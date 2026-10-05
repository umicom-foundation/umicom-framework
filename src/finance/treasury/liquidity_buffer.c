/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_buffer.c
 *
 * PURPOSE:
 *   Implement compare available liquidity against a policy buffer requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_buffer.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury liquidity buffer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_buffer_init(UmiTreasuryLiquidityBuffer *value,
    const char *id,
    int64_t available_minor,
    int64_t required_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->available_minor=available_minor;
    value->required_minor=required_minor;
    return umi_treasury_liquidity_buffer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury liquidity buffer satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_liquidity_buffer_valid(const UmiTreasuryLiquidityBuffer *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->available_minor >= 0 && value->required_minor >= 0);
}

/*
 * Provide the treasury liquidity buffer surplus minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_liquidity_buffer_surplus_minor(const UmiTreasuryLiquidityBuffer *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->available_minor - value->required_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryLiquidityBufferArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0e76d3ec514f2c0d);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryLiquidityBuffer *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryLiquidityBufferArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryLiquidityBuffer *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryLiquidityBufferArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryLiquidityBuffer *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->available_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->required_minor);
}
static void UmiTreasuryLiquidityBufferArchiveRead(UmiArchiveReader *reader, UmiTreasuryLiquidityBuffer *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->available_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->required_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryLiquidityBufferArchiveValidate(const UmiTreasuryLiquidityBuffer *value)
{
    return umi_treasury_liquidity_buffer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_liquidity_buffer_archive_encode, umi_treasury_liquidity_buffer_archive_decode,
    UmiTreasuryLiquidityBuffer, UmiTreasuryLiquidityBufferArchiveSchema, UmiTreasuryLiquidityBufferArchiveBound, UmiTreasuryLiquidityBufferArchiveWrite, UmiTreasuryLiquidityBufferArchiveRead, UmiTreasuryLiquidityBufferArchiveValidate)
