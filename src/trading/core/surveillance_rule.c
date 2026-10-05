/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/surveillance_rule.c
 *
 * PURPOSE:
 *   Define reusable market-surveillance thresholds and alert severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/surveillance_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define reusable market-surveillance thresholds and alert severity.. */
UmiStatus umi_trading_surveillance_rule_init(UmiTradingSurveillanceRule *value,uint32_t threshold, uint32_t window_seconds, UmiTradingCoreSeverity severity) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->threshold=threshold;
    value->window_seconds=window_seconds;
    value->severity=severity;
    return umi_trading_surveillance_rule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_surveillance_rule_valid(const UmiTradingSurveillanceRule *value) { return value!=NULL && (value->threshold>0U && value->window_seconds>0U && value->severity>=UMI_TRADING_CORE_INFO && value->severity<=UMI_TRADING_CORE_CRITICAL); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingSurveillanceRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x14130691c7439ce3);

    return schema;
}
static size_t UmiTradingSurveillanceRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingSurveillanceRuleArchiveWrite(UmiArchiveWriter *writer, const UmiTradingSurveillanceRule *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->threshold);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->window_seconds);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
}
static void UmiTradingSurveillanceRuleArchiveRead(UmiArchiveReader *reader, UmiTradingSurveillanceRule *value)
{
    value->threshold = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->window_seconds = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->severity = (UmiTradingCoreSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTradingSurveillanceRuleArchiveValidate(const UmiTradingSurveillanceRule *value)
{
    return umi_trading_surveillance_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_surveillance_rule_archive_encode, umi_trading_surveillance_rule_archive_decode,
    UmiTradingSurveillanceRule, UmiTradingSurveillanceRuleArchiveSchema, UmiTradingSurveillanceRuleArchiveBound, UmiTradingSurveillanceRuleArchiveWrite, UmiTradingSurveillanceRuleArchiveRead, UmiTradingSurveillanceRuleArchiveValidate)
