/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/matching_policy.c
 *
 * PURPOSE:
 *   Define common exchange matching priorities and self-trade prevention behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/matching_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define common exchange matching priorities and self-trade prevention behaviour.. */
UmiStatus umi_trading_matching_policy_init(UmiTradingMatchingPolicy *value,bool price_time_priority, bool prevent_self_trade, uint32_t max_matches_per_cycle) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->price_time_priority=price_time_priority;
    value->prevent_self_trade=prevent_self_trade;
    value->max_matches_per_cycle=max_matches_per_cycle;
    return umi_trading_matching_policy_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_matching_policy_valid(const UmiTradingMatchingPolicy *value) { return value!=NULL && (value->price_time_priority && value->max_matches_per_cycle>0U && value->max_matches_per_cycle<=UMI_TRADING_CORE_MAX_EVENTS); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingMatchingPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2f618b230563503a);

    return schema;
}
static size_t UmiTradingMatchingPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingMatchingPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiTradingMatchingPolicy *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->price_time_priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->prevent_self_trade);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_matches_per_cycle);
}
static void UmiTradingMatchingPolicyArchiveRead(UmiArchiveReader *reader, UmiTradingMatchingPolicy *value)
{
    value->price_time_priority = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->prevent_self_trade = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->max_matches_per_cycle = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTradingMatchingPolicyArchiveValidate(const UmiTradingMatchingPolicy *value)
{
    return umi_trading_matching_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_matching_policy_archive_encode, umi_trading_matching_policy_archive_decode,
    UmiTradingMatchingPolicy, UmiTradingMatchingPolicyArchiveSchema, UmiTradingMatchingPolicyArchiveBound, UmiTradingMatchingPolicyArchiveWrite, UmiTradingMatchingPolicyArchiveRead, UmiTradingMatchingPolicyArchiveValidate)
