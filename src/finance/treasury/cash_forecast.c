/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_forecast.c
 *
 * PURPOSE:
 *   Implement aggregate expected cash inflows and outflows for a forecast horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_forecast.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury cash forecast from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_forecast_init(UmiTreasuryCashForecast *value,
    const char *id,
    int64_t horizon_end_epoch_millis,
    int64_t inflow_minor,
    int64_t outflow_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->horizon_end_epoch_millis=horizon_end_epoch_millis;
    value->inflow_minor=inflow_minor;
    value->outflow_minor=outflow_minor;
    return umi_treasury_cash_forecast_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury cash forecast satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_forecast_valid(const UmiTreasuryCashForecast *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->horizon_end_epoch_millis >= 0 && value->inflow_minor >= 0 && value->outflow_minor >= 0);
}

/*
 * Provide the treasury cash forecast net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_forecast_net_minor(const UmiTreasuryCashForecast *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->inflow_minor - value->outflow_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCashForecastArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf0c73d082c4dcaee);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCashForecast *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCashForecastArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCashForecast *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryCashForecastArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCashForecast *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->horizon_end_epoch_millis);
    UmiArchiveWriteSigned(writer, (int64_t)value->inflow_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->outflow_minor);
}
static void UmiTreasuryCashForecastArchiveRead(UmiArchiveReader *reader, UmiTreasuryCashForecast *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->horizon_end_epoch_millis = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->inflow_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->outflow_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCashForecastArchiveValidate(const UmiTreasuryCashForecast *value)
{
    return umi_treasury_cash_forecast_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_cash_forecast_archive_encode, umi_treasury_cash_forecast_archive_decode,
    UmiTreasuryCashForecast, UmiTreasuryCashForecastArchiveSchema, UmiTreasuryCashForecastArchiveBound, UmiTreasuryCashForecastArchiveWrite, UmiTreasuryCashForecastArchiveRead, UmiTreasuryCashForecastArchiveValidate)
