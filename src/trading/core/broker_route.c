/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/broker_route.c
 *
 * PURPOSE:
 *   Describe a candidate broker/venue route with cost and latency scores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate describe a candidate broker/venue route with cost and latency scores.. */
UmiStatus umi_trading_broker_route_init(UmiTradingBrokerRoute *value,const UmiFinancialId * route_id, const UmiFinancialId * venue_id, uint32_t cost_bps, uint32_t latency_score, bool enabled) {
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
    if(route_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->route_id=*route_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->cost_bps=cost_bps;
    value->latency_score=latency_score;
    value->enabled=enabled;
    return umi_trading_broker_route_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_broker_route_valid(const UmiTradingBrokerRoute *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->route_id.value, '\0', sizeof(value->route_id.value)) == NULL) return 0;
    if (memchr(value->venue_id.value, '\0', sizeof(value->venue_id.value)) == NULL) return 0;
 return value!=NULL && (value->route_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->cost_bps<=10000U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingBrokerRouteArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x355bf1885ba36271);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingBrokerRoute *)0)->route_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingBrokerRoute *)0)->venue_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingBrokerRouteArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingBrokerRoute *)0)->route_id.value) - 1U +
        8U + sizeof(((UmiTradingBrokerRoute *)0)->venue_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTradingBrokerRouteArchiveWrite(UmiArchiveWriter *writer, const UmiTradingBrokerRoute *value)
{
    UmiArchiveWriteText(writer, value->route_id.value, sizeof(value->route_id.value));
    UmiArchiveWriteText(writer, value->venue_id.value, sizeof(value->venue_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cost_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->latency_score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiTradingBrokerRouteArchiveRead(UmiArchiveReader *reader, UmiTradingBrokerRoute *value)
{
    UmiArchiveReadText(reader, value->route_id.value, sizeof(value->route_id.value));
    UmiArchiveReadText(reader, value->venue_id.value, sizeof(value->venue_id.value));
    value->cost_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->latency_score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTradingBrokerRouteArchiveValidate(const UmiTradingBrokerRoute *value)
{
    return umi_trading_broker_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_broker_route_archive_encode, umi_trading_broker_route_archive_decode,
    UmiTradingBrokerRoute, UmiTradingBrokerRouteArchiveSchema, UmiTradingBrokerRouteArchiveBound, UmiTradingBrokerRouteArchiveWrite, UmiTradingBrokerRouteArchiveRead, UmiTradingBrokerRouteArchiveValidate)
