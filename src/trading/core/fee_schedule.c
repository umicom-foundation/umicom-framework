/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/fee_schedule.c
 *
 * PURPOSE:
 *   Define maker/taker exchange fees in minor units per lot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_schedule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define maker/taker exchange fees in minor units per lot.. */
UmiStatus umi_trading_fee_schedule_init(UmiTradingFeeSchedule *value,int64_t maker_minor_per_lot, int64_t taker_minor_per_lot, int64_t regulatory_minor_per_lot) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->maker_minor_per_lot=maker_minor_per_lot;
    value->taker_minor_per_lot=taker_minor_per_lot;
    value->regulatory_minor_per_lot=regulatory_minor_per_lot;
    return umi_trading_fee_schedule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_fee_schedule_valid(const UmiTradingFeeSchedule *value) { return value!=NULL && (value->maker_minor_per_lot>=0 && value->taker_minor_per_lot>=0 && value->regulatory_minor_per_lot>=0); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingFeeScheduleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4a3ec040e545018d);

    return schema;
}
static size_t UmiTradingFeeScheduleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingFeeScheduleArchiveWrite(UmiArchiveWriter *writer, const UmiTradingFeeSchedule *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->maker_minor_per_lot);
    UmiArchiveWriteSigned(writer, (int64_t)value->taker_minor_per_lot);
    UmiArchiveWriteSigned(writer, (int64_t)value->regulatory_minor_per_lot);
}
static void UmiTradingFeeScheduleArchiveRead(UmiArchiveReader *reader, UmiTradingFeeSchedule *value)
{
    value->maker_minor_per_lot = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->taker_minor_per_lot = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->regulatory_minor_per_lot = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTradingFeeScheduleArchiveValidate(const UmiTradingFeeSchedule *value)
{
    return umi_trading_fee_schedule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_fee_schedule_archive_encode, umi_trading_fee_schedule_archive_decode,
    UmiTradingFeeSchedule, UmiTradingFeeScheduleArchiveSchema, UmiTradingFeeScheduleArchiveBound, UmiTradingFeeScheduleArchiveWrite, UmiTradingFeeScheduleArchiveRead, UmiTradingFeeScheduleArchiveValidate)
