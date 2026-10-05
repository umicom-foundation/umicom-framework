/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/commission_schedule.c
 *
 * PURPOSE:
 *   Define per-lot and minimum brokerage commission in integer minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/commission_schedule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define per-lot and minimum brokerage commission in integer minor units.. */
UmiStatus umi_trading_commission_schedule_init(UmiTradingCommissionSchedule *value,int64_t per_lot_minor, int64_t minimum_minor, int64_t maximum_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->per_lot_minor=per_lot_minor;
    value->minimum_minor=minimum_minor;
    value->maximum_minor=maximum_minor;
    return umi_trading_commission_schedule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_commission_schedule_valid(const UmiTradingCommissionSchedule *value) { return value!=NULL && (value->per_lot_minor>=0 && value->minimum_minor>=0 && value->maximum_minor>=value->minimum_minor); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingCommissionScheduleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcf62213eeaf1d5e8);

    return schema;
}
static size_t UmiTradingCommissionScheduleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingCommissionScheduleArchiveWrite(UmiArchiveWriter *writer, const UmiTradingCommissionSchedule *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->per_lot_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum_minor);
}
static void UmiTradingCommissionScheduleArchiveRead(UmiArchiveReader *reader, UmiTradingCommissionSchedule *value)
{
    value->per_lot_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->minimum_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->maximum_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTradingCommissionScheduleArchiveValidate(const UmiTradingCommissionSchedule *value)
{
    return umi_trading_commission_schedule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_commission_schedule_archive_encode, umi_trading_commission_schedule_archive_decode,
    UmiTradingCommissionSchedule, UmiTradingCommissionScheduleArchiveSchema, UmiTradingCommissionScheduleArchiveBound, UmiTradingCommissionScheduleArchiveWrite, UmiTradingCommissionScheduleArchiveRead, UmiTradingCommissionScheduleArchiveValidate)
