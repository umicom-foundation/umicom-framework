/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/netting_result.c
 *
 * PURPOSE:
 *   Implement record gross and net exposure reduction from netting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_result.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury netting result from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_netting_result_init(UmiTreasuryNettingResult *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    return umi_treasury_netting_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury netting result satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_netting_result_valid(const UmiTreasuryNettingResult *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->gross_minor >= 0 && umi_treasury_abs_i64(value->net_minor) <= value->gross_minor);
}

/*
 * Provide the treasury netting result reduction minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_netting_result_reduction_minor(const UmiTreasuryNettingResult *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->gross_minor - umi_treasury_abs_i64(value->net_minor);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryNettingResultArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x54e229059cf05e49);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryNettingResult *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryNettingResultArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryNettingResult *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryNettingResultArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryNettingResult *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->gross_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->net_minor);
}
static void UmiTreasuryNettingResultArchiveRead(UmiArchiveReader *reader, UmiTreasuryNettingResult *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->gross_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->net_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryNettingResultArchiveValidate(const UmiTreasuryNettingResult *value)
{
    return umi_treasury_netting_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_netting_result_archive_encode, umi_treasury_netting_result_archive_decode,
    UmiTreasuryNettingResult, UmiTreasuryNettingResultArchiveSchema, UmiTreasuryNettingResultArchiveBound, UmiTreasuryNettingResultArchiveWrite, UmiTreasuryNettingResultArchiveRead, UmiTreasuryNettingResultArchiveValidate)
