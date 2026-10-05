/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_status.c
 *
 * PURPOSE:
 *   Capture exchange phase, sequence and operational availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_status.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate capture exchange phase, sequence and operational availability.. */
UmiStatus umi_trading_market_status_init(UmiTradingMarketStatus *value,UmiTradingCoreMarketPhase phase, uint64_t sequence, bool operational) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->phase=phase;
    value->sequence=sequence;
    value->operational=operational;
    return umi_trading_market_status_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_market_status_valid(const UmiTradingMarketStatus *value) { return value!=NULL && (value->phase>=UMI_TRADING_CORE_PHASE_CLOSED && value->phase<=UMI_TRADING_CORE_PHASE_POSTCLOSE && value->sequence>0U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingMarketStatusArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa60f8ba4d36ac694);

    return schema;
}
static size_t UmiTradingMarketStatusArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingMarketStatusArchiveWrite(UmiArchiveWriter *writer, const UmiTradingMarketStatus *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->operational);
}
static void UmiTradingMarketStatusArchiveRead(UmiArchiveReader *reader, UmiTradingMarketStatus *value)
{
    value->phase = (UmiTradingCoreMarketPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->operational = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTradingMarketStatusArchiveValidate(const UmiTradingMarketStatus *value)
{
    return umi_trading_market_status_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_market_status_archive_encode, umi_trading_market_status_archive_decode,
    UmiTradingMarketStatus, UmiTradingMarketStatusArchiveSchema, UmiTradingMarketStatusArchiveBound, UmiTradingMarketStatusArchiveWrite, UmiTradingMarketStatusArchiveRead, UmiTradingMarketStatusArchiveValidate)
