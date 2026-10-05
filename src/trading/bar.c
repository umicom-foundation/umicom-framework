/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/bar.c
 *
 * PURPOSE:
 *   Validate OHLCV bars and calculate range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of bar. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/bar.h"
#include "../base/value_archive_internal.h"

#include <math.h>

/* Check every numeric input before comparisons. A non-finite market value can
 * otherwise pass ordinary range tests and later damage scales or indicators. */
int umi_bar_valid(const UmiBar *bar)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (bar == NULL) return 0;
    if (memchr(bar->instrument.instrument_id.value, '\0', sizeof(bar->instrument.instrument_id.value)) == NULL) return 0;
    if (memchr(bar->instrument.symbol, '\0', sizeof(bar->instrument.symbol)) == NULL) return 0;
    if (memchr(bar->instrument.venue, '\0', sizeof(bar->instrument.venue)) == NULL) return 0;
    if (memchr(bar->instrument.currency.code, '\0', sizeof(bar->instrument.currency.code)) == NULL) return 0;

    if (bar == NULL || !isfinite(bar->open) || !isfinite(bar->high) ||
        !isfinite(bar->low) || !isfinite(bar->close) ||
        !isfinite(bar->volume)) {
        return 0;
    }
    return bar->high >= bar->low && bar->high >= bar->open &&
           bar->high >= bar->close && bar->low <= bar->open &&
           bar->low <= bar->close && bar->volume >= 0.0 &&
           bar->end_time_ms >= bar->start_time_ms;
}
/* Provide the bar range operation used by this module and its client applications. */
double umi_bar_range(const UmiBar *b){return umi_bar_valid(b)?b->high-b->low:0.0;}
/* Provide the bar typical price operation used by this module and its client applications. */
double umi_bar_typical_price(const UmiBar *b){return umi_bar_valid(b)?(b->high+b->low+b->close)/3.0:0.0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBarArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x33b6af332007e283);
    schema = (schema ^ (uint64_t)sizeof(((UmiBar *)0)->instrument.instrument_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBar *)0)->instrument.symbol)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBar *)0)->instrument.venue)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBar *)0)->instrument.currency.code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBarArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBar *)0)->instrument.instrument_id.value) - 1U +
        8U + sizeof(((UmiBar *)0)->instrument.symbol) - 1U +
        8U + sizeof(((UmiBar *)0)->instrument.venue) - 1U +
        8U + sizeof(((UmiBar *)0)->instrument.currency.code) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiBarArchiveWrite(UmiArchiveWriter *writer, const UmiBar *value)
{
    UmiArchiveWriteText(writer, value->instrument.instrument_id.value, sizeof(value->instrument.instrument_id.value));
    UmiArchiveWriteText(writer, value->instrument.symbol, sizeof(value->instrument.symbol));
    UmiArchiveWriteText(writer, value->instrument.venue, sizeof(value->instrument.venue));
    UmiArchiveWriteText(writer, value->instrument.currency.code, sizeof(value->instrument.currency.code));
    UmiArchiveWriteDouble(writer, value->instrument.multiplier);
    UmiArchiveWriteSigned(writer, (int64_t)value->instrument.expiry_yyyymmdd);
    UmiArchiveWriteDouble(writer, value->open);
    UmiArchiveWriteDouble(writer, value->high);
    UmiArchiveWriteDouble(writer, value->low);
    UmiArchiveWriteDouble(writer, value->close);
    UmiArchiveWriteDouble(writer, value->volume);
    UmiArchiveWriteSigned(writer, (int64_t)value->start_time_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->end_time_ms);
}
static void UmiBarArchiveRead(UmiArchiveReader *reader, UmiBar *value)
{
    UmiArchiveReadText(reader, value->instrument.instrument_id.value, sizeof(value->instrument.instrument_id.value));
    UmiArchiveReadText(reader, value->instrument.symbol, sizeof(value->instrument.symbol));
    UmiArchiveReadText(reader, value->instrument.venue, sizeof(value->instrument.venue));
    UmiArchiveReadText(reader, value->instrument.currency.code, sizeof(value->instrument.currency.code));
    value->instrument.multiplier = UmiArchiveReadDouble(reader);
    value->instrument.expiry_yyyymmdd = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->open = UmiArchiveReadDouble(reader);
    value->high = UmiArchiveReadDouble(reader);
    value->low = UmiArchiveReadDouble(reader);
    value->close = UmiArchiveReadDouble(reader);
    value->volume = UmiArchiveReadDouble(reader);
    value->start_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->end_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiBarArchiveValidate(const UmiBar *value)
{
    return umi_bar_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_bar_archive_encode, umi_bar_archive_decode,
    UmiBar, UmiBarArchiveSchema, UmiBarArchiveBound, UmiBarArchiveWrite, UmiBarArchiveRead, UmiBarArchiveValidate)
