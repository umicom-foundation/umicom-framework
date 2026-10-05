/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_event.c
 *
 * PURPOSE:
 *   Capture sequence-ordered evidence for an order lifecycle transition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_event.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate capture sequence-ordered evidence for an order lifecycle transition.. */
UmiStatus umi_trading_order_event_init(UmiTradingOrderEvent *value,const UmiFinancialId * client_order_id, uint64_t sequence, int64_t event_time_ms, UmiTradingCoreOrderState state) {
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
    if(client_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->client_order_id=*client_order_id;
    value->sequence=sequence;
    value->event_time_ms=event_time_ms;
    value->state=state;
    return umi_trading_order_event_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_event_valid(const UmiTradingOrderEvent *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->client_order_id.value, '\0', sizeof(value->client_order_id.value)) == NULL) return 0;
 return value!=NULL && (value->client_order_id.value[0]!='\0' && value->sequence>0U && value->event_time_ms>=0 && value->state>=UMI_TRADING_CORE_ORDER_PENDING_NEW && value->state<=UMI_TRADING_CORE_ORDER_EXPIRED); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingOrderEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd8ac85b3b47736ad);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingOrderEvent *)0)->client_order_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingOrderEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingOrderEvent *)0)->client_order_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTradingOrderEventArchiveWrite(UmiArchiveWriter *writer, const UmiTradingOrderEvent *value)
{
    UmiArchiveWriteText(writer, value->client_order_id.value, sizeof(value->client_order_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteSigned(writer, (int64_t)value->event_time_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
}
static void UmiTradingOrderEventArchiveRead(UmiArchiveReader *reader, UmiTradingOrderEvent *value)
{
    UmiArchiveReadText(reader, value->client_order_id.value, sizeof(value->client_order_id.value));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->event_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->state = (UmiTradingCoreOrderState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTradingOrderEventArchiveValidate(const UmiTradingOrderEvent *value)
{
    return umi_trading_order_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_order_event_archive_encode, umi_trading_order_event_archive_decode,
    UmiTradingOrderEvent, UmiTradingOrderEventArchiveSchema, UmiTradingOrderEventArchiveBound, UmiTradingOrderEventArchiveWrite, UmiTradingOrderEventArchiveRead, UmiTradingOrderEventArchiveValidate)
