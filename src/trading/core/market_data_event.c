/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_data_event.c
 *
 * PURPOSE:
 *   Normalise venue market-data sequence, instrument identity and event time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_data_event.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate normalise venue market-data sequence, instrument identity and event time.. */
UmiStatus umi_trading_market_data_event_init(UmiTradingMarketDataEvent *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, uint64_t sequence, int64_t event_time_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(instrument_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->instrument_id=*instrument_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->sequence=sequence;
    value->event_time_ms=event_time_ms;
    return umi_trading_market_data_event_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_market_data_event_valid(const UmiTradingMarketDataEvent *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->instrument_id.value, '\0', sizeof(value->instrument_id.value)) == NULL) return 0;
    if (memchr(value->venue_id.value, '\0', sizeof(value->venue_id.value)) == NULL) return 0;
 return value!=NULL && (value->instrument_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->sequence>0U && value->event_time_ms>=0); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingMarketDataEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x47ba2515d9dbcb48);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingMarketDataEvent *)0)->instrument_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingMarketDataEvent *)0)->venue_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingMarketDataEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingMarketDataEvent *)0)->instrument_id.value) - 1U +
        8U + sizeof(((UmiTradingMarketDataEvent *)0)->venue_id.value) - 1U +
        8U +
        8U;
}
static void UmiTradingMarketDataEventArchiveWrite(UmiArchiveWriter *writer, const UmiTradingMarketDataEvent *value)
{
    UmiArchiveWriteText(writer, value->instrument_id.value, sizeof(value->instrument_id.value));
    UmiArchiveWriteText(writer, value->venue_id.value, sizeof(value->venue_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteSigned(writer, (int64_t)value->event_time_ms);
}
static void UmiTradingMarketDataEventArchiveRead(UmiArchiveReader *reader, UmiTradingMarketDataEvent *value)
{
    UmiArchiveReadText(reader, value->instrument_id.value, sizeof(value->instrument_id.value));
    UmiArchiveReadText(reader, value->venue_id.value, sizeof(value->venue_id.value));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->event_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTradingMarketDataEventArchiveValidate(const UmiTradingMarketDataEvent *value)
{
    return umi_trading_market_data_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_market_data_event_archive_encode, umi_trading_market_data_event_archive_decode,
    UmiTradingMarketDataEvent, UmiTradingMarketDataEventArchiveSchema, UmiTradingMarketDataEventArchiveBound, UmiTradingMarketDataEventArchiveWrite, UmiTradingMarketDataEventArchiveRead, UmiTradingMarketDataEventArchiveValidate)
