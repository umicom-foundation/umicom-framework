/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/execution_policy.c
 *
 * PURPOSE:
 *   Define venue-count, participation and urgency bounds for execution strategies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise and validate define venue-count, participation and urgency bounds for execution strategies.. */
UmiStatus umi_trading_execution_policy_init(UmiTradingExecutionPolicy *value,uint32_t max_venues, uint32_t participation_bps, uint32_t urgency) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->max_venues=max_venues;
    value->participation_bps=participation_bps;
    value->urgency=urgency;
    return umi_trading_execution_policy_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_execution_policy_valid(const UmiTradingExecutionPolicy *value) { return value!=NULL && (value->max_venues>0U && value->max_venues<=UMI_TRADING_CORE_MAX_ITEMS && value->participation_bps<=10000U && value->urgency<=100U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingExecutionPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x709ac31739be94b1);

    return schema;
}
static size_t UmiTradingExecutionPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiTradingExecutionPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiTradingExecutionPolicy *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_venues);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->participation_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->urgency);
}
static void UmiTradingExecutionPolicyArchiveRead(UmiArchiveReader *reader, UmiTradingExecutionPolicy *value)
{
    value->max_venues = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->participation_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->urgency = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTradingExecutionPolicyArchiveValidate(const UmiTradingExecutionPolicy *value)
{
    return umi_trading_execution_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_execution_policy_archive_encode, umi_trading_execution_policy_archive_decode,
    UmiTradingExecutionPolicy, UmiTradingExecutionPolicyArchiveSchema, UmiTradingExecutionPolicyArchiveBound, UmiTradingExecutionPolicyArchiveWrite, UmiTradingExecutionPolicyArchiveRead, UmiTradingExecutionPolicyArchiveValidate)
