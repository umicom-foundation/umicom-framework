/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/margin_profile.c
 *
 * PURPOSE:
 *   Define conservative initial and maintenance margin ratios in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/margin_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define conservative initial and maintenance margin ratios in basis points.. */
UmiStatus umi_trading_margin_profile_init(UmiTradingMarginProfile *value,uint32_t initial_margin_bps, uint32_t maintenance_margin_bps, uint32_t concentration_addon_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->initial_margin_bps=initial_margin_bps;
    value->maintenance_margin_bps=maintenance_margin_bps;
    value->concentration_addon_bps=concentration_addon_bps;
    return umi_trading_margin_profile_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_margin_profile_valid(const UmiTradingMarginProfile *value) { return value!=NULL && (value->initial_margin_bps<=10000U && value->maintenance_margin_bps<=value->initial_margin_bps && value->concentration_addon_bps<=10000U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingMarginProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x456d5e271ec242cb);

    return schema;
}
static size_t UmiTradingMarginProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingMarginProfileArchiveWrite(UmiArchiveWriter *writer, const UmiTradingMarginProfile *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->initial_margin_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maintenance_margin_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->concentration_addon_bps);
}
static void UmiTradingMarginProfileArchiveRead(UmiArchiveReader *reader, UmiTradingMarginProfile *value)
{
    value->initial_margin_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maintenance_margin_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->concentration_addon_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTradingMarginProfileArchiveValidate(const UmiTradingMarginProfile *value)
{
    return umi_trading_margin_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_margin_profile_archive_encode, umi_trading_margin_profile_archive_decode,
    UmiTradingMarginProfile, UmiTradingMarginProfileArchiveSchema, UmiTradingMarginProfileArchiveBound, UmiTradingMarginProfileArchiveWrite, UmiTradingMarginProfileArchiveRead, UmiTradingMarginProfileArchiveValidate)
