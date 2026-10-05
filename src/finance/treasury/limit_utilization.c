/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/limit_utilization.c
 *
 * PURPOSE:
 *   Implement calculate risk-limit utilisation using basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_utilization.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury limit utilization from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_limit_utilization_init(UmiTreasuryLimitUtilization *value,
    const char *id,
    int64_t used_minor,
    int64_t limit_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->used_minor=used_minor;
    value->limit_minor=limit_minor;
    return umi_treasury_limit_utilization_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury limit utilization satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_limit_utilization_valid(const UmiTreasuryLimitUtilization *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->used_minor >= 0 && value->limit_minor > 0);
}

/*
 * Provide the treasury limit utilization utilization bps operation used by this module and
 * its client applications.
 */
uint32_t umi_treasury_limit_utilization_utilization_bps(const UmiTreasuryLimitUtilization *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint32_t)0;
    return (uint32_t)((value->used_minor * 10000) / value->limit_minor);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryLimitUtilizationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x21a045161041fcaa);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryLimitUtilization *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryLimitUtilizationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryLimitUtilization *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryLimitUtilizationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryLimitUtilization *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->used_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->limit_minor);
}
static void UmiTreasuryLimitUtilizationArchiveRead(UmiArchiveReader *reader, UmiTreasuryLimitUtilization *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->used_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->limit_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryLimitUtilizationArchiveValidate(const UmiTreasuryLimitUtilization *value)
{
    return umi_treasury_limit_utilization_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_limit_utilization_archive_encode, umi_treasury_limit_utilization_archive_decode,
    UmiTreasuryLimitUtilization, UmiTreasuryLimitUtilizationArchiveSchema, UmiTreasuryLimitUtilizationArchiveBound, UmiTreasuryLimitUtilizationArchiveWrite, UmiTreasuryLimitUtilizationArchiveRead, UmiTreasuryLimitUtilizationArchiveValidate)
