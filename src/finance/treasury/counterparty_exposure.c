/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/counterparty_exposure.c
 *
 * PURPOSE:
 *   Implement represent counterparty current and potential future exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/counterparty_exposure.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury counterparty exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_counterparty_exposure_init(UmiTreasuryCounterpartyExposure *value,
    const char *id,
    int64_t current_minor,
    int64_t potential_future_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->current_minor=current_minor;
    value->potential_future_minor=potential_future_minor;
    return umi_treasury_counterparty_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury counterparty exposure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_counterparty_exposure_valid(const UmiTreasuryCounterpartyExposure *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->current_minor >= 0 && value->potential_future_minor >= 0);
}

/*
 * Provide the treasury counterparty exposure total minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_counterparty_exposure_total_minor(const UmiTreasuryCounterpartyExposure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->current_minor + value->potential_future_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCounterpartyExposureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1b5abbcd6c2acaa5);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCounterpartyExposure *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCounterpartyExposureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCounterpartyExposure *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryCounterpartyExposureArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCounterpartyExposure *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->current_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->potential_future_minor);
}
static void UmiTreasuryCounterpartyExposureArchiveRead(UmiArchiveReader *reader, UmiTreasuryCounterpartyExposure *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->current_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->potential_future_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCounterpartyExposureArchiveValidate(const UmiTreasuryCounterpartyExposure *value)
{
    return umi_treasury_counterparty_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_counterparty_exposure_archive_encode, umi_treasury_counterparty_exposure_archive_decode,
    UmiTreasuryCounterpartyExposure, UmiTreasuryCounterpartyExposureArchiveSchema, UmiTreasuryCounterpartyExposureArchiveBound, UmiTreasuryCounterpartyExposureArchiveWrite, UmiTreasuryCounterpartyExposureArchiveRead, UmiTreasuryCounterpartyExposureArchiveValidate)
