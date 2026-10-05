/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_cancel.c
 *
 * PURPOSE:
 *   Describe a cancellable order request with version control and reason code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_cancel.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate describe a cancellable order request with version control and reason code.. */
UmiStatus umi_trading_order_cancel_init(UmiTradingOrderCancel *value,const UmiFinancialId * client_order_id, uint64_t expected_version, uint32_t reason_code) {
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
    value->expected_version=expected_version;
    value->reason_code=reason_code;
    return umi_trading_order_cancel_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_cancel_valid(const UmiTradingOrderCancel *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->client_order_id.value, '\0', sizeof(value->client_order_id.value)) == NULL) return 0;
 return value!=NULL && (value->client_order_id.value[0]!='\0' && value->expected_version>0U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingOrderCancelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x468611290ae1d353);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingOrderCancel *)0)->client_order_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingOrderCancelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingOrderCancel *)0)->client_order_id.value) - 1U +
        8U +
        8U;
}
static void UmiTradingOrderCancelArchiveWrite(UmiArchiveWriter *writer, const UmiTradingOrderCancel *value)
{
    UmiArchiveWriteText(writer, value->client_order_id.value, sizeof(value->client_order_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->expected_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reason_code);
}
static void UmiTradingOrderCancelArchiveRead(UmiArchiveReader *reader, UmiTradingOrderCancel *value)
{
    UmiArchiveReadText(reader, value->client_order_id.value, sizeof(value->client_order_id.value));
    value->expected_version = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->reason_code = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTradingOrderCancelArchiveValidate(const UmiTradingOrderCancel *value)
{
    return umi_trading_order_cancel_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_order_cancel_archive_encode, umi_trading_order_cancel_archive_decode,
    UmiTradingOrderCancel, UmiTradingOrderCancelArchiveSchema, UmiTradingOrderCancelArchiveBound, UmiTradingOrderCancelArchiveWrite, UmiTradingOrderCancelArchiveRead, UmiTradingOrderCancelArchiveValidate)
