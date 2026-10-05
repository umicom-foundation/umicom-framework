/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_session.c
 *
 * PURPOSE:
 *   Model a bounded venue trading session and its current phase.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_session.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate model a bounded venue trading session and its current phase.. */
UmiStatus umi_trading_venue_session_init(UmiTradingVenueSession *value,const UmiFinancialId * venue_id, int64_t open_time_ms, int64_t close_time_ms, UmiTradingCoreMarketPhase phase) {
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
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->open_time_ms=open_time_ms;
    value->close_time_ms=close_time_ms;
    value->phase=phase;
    return umi_trading_venue_session_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_venue_session_valid(const UmiTradingVenueSession *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->venue_id.value, '\0', sizeof(value->venue_id.value)) == NULL) return 0;
 return value!=NULL && (value->venue_id.value[0]!='\0' && value->open_time_ms>=0 && value->close_time_ms>value->open_time_ms && value->phase>=UMI_TRADING_CORE_PHASE_CLOSED && value->phase<=UMI_TRADING_CORE_PHASE_POSTCLOSE); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingVenueSessionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8151350a8c316a18);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingVenueSession *)0)->venue_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingVenueSessionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingVenueSession *)0)->venue_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTradingVenueSessionArchiveWrite(UmiArchiveWriter *writer, const UmiTradingVenueSession *value)
{
    UmiArchiveWriteText(writer, value->venue_id.value, sizeof(value->venue_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->open_time_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->close_time_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
}
static void UmiTradingVenueSessionArchiveRead(UmiArchiveReader *reader, UmiTradingVenueSession *value)
{
    UmiArchiveReadText(reader, value->venue_id.value, sizeof(value->venue_id.value));
    value->open_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->close_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->phase = (UmiTradingCoreMarketPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTradingVenueSessionArchiveValidate(const UmiTradingVenueSession *value)
{
    return umi_trading_venue_session_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_venue_session_archive_encode, umi_trading_venue_session_archive_decode,
    UmiTradingVenueSession, UmiTradingVenueSessionArchiveSchema, UmiTradingVenueSessionArchiveBound, UmiTradingVenueSessionArchiveWrite, UmiTradingVenueSessionArchiveRead, UmiTradingVenueSessionArchiveValidate)
