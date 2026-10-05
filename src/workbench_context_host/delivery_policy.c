/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/delivery_policy.c
 *
 * PURPOSE:
 *   Provide safe default delivery pressure rules and validate custom policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/delivery_policy.h"
#include "../base/value_archive_internal.h"
/*
 * Provide the workbench context host delivery policy default operation used by this module
 * and its client applications.
 */
UmiWorkbenchContextHostDeliveryPolicy umi_workbench_context_host_delivery_policy_default(void)
{
    UmiWorkbenchContextHostDeliveryPolicy p;
    p.max_pending_per_endpoint=32U;p.overflow_mode=UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST;
    p.coalesce_same_kind=false;p.coalesce_same_context=true;p.reject_expired=true;p.revision=1U;return p;
}
/*
 * Check that workbench context host delivery policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_delivery_policy_validate(
    const UmiWorkbenchContextHostDeliveryPolicy *policy)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if(!policy||policy->max_pending_per_endpoint==0U||
       policy->max_pending_per_endpoint>UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if(policy->overflow_mode<UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_DROP_OLDEST||
       policy->overflow_mode>UMI_WORKBENCH_CONTEXT_HOST_OVERFLOW_REJECT)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchContextHostDeliveryPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x89a80318f96aa633);

    return schema;
}
static size_t UmiWorkbenchContextHostDeliveryPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextHostDeliveryPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextHostDeliveryPolicy *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_pending_per_endpoint);
    UmiArchiveWriteSigned(writer, (int64_t)value->overflow_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->coalesce_same_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->coalesce_same_context);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reject_expired);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextHostDeliveryPolicyArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextHostDeliveryPolicy *value)
{
    value->max_pending_per_endpoint = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->overflow_mode = (UmiWorkbenchContextHostOverflowMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->coalesce_same_kind = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->coalesce_same_context = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->reject_expired = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextHostDeliveryPolicyArchiveValidate(const UmiWorkbenchContextHostDeliveryPolicy *value)
{
    return umi_workbench_context_host_delivery_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_host_delivery_policy_archive_encode, umi_workbench_context_host_delivery_policy_archive_decode,
    UmiWorkbenchContextHostDeliveryPolicy, UmiWorkbenchContextHostDeliveryPolicyArchiveSchema, UmiWorkbenchContextHostDeliveryPolicyArchiveBound, UmiWorkbenchContextHostDeliveryPolicyArchiveWrite, UmiWorkbenchContextHostDeliveryPolicyArchiveRead, UmiWorkbenchContextHostDeliveryPolicyArchiveValidate)
