/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/delivery_policy.c
 *
 * PURPOSE:
 *   Describe acknowledgement, attempt and durability requirements for message delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/delivery_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric delivery policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_delivery_policy_init(UmiFabricDeliveryPolicy *item, const char *policy_id, UmiFabricDeliveryMode mode, uint32_t max_attempts, uint64_t acknowledgement_timeout_ms, bool durable) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->mode=mode;item->max_attempts=max_attempts;item->acknowledgement_timeout_ms=acknowledgement_timeout_ms;item->durable=durable;
    return umi_fabric_delivery_policy_validate(item);
}
/*
 * Check that fabric delivery policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_delivery_policy_validate(const UmiFabricDeliveryPolicy *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->policy_id, '\0', sizeof(item->policy_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->policy_id[0]!='\0' && item->mode>=UMI_FABRIC_DELIVERY_AT_MOST_ONCE && item->mode<=UMI_FABRIC_DELIVERY_IDEMPOTENT_EFFECT && item->max_attempts>0U && item->acknowledgement_timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricDeliveryPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x23a1080f1c61751a);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricDeliveryPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricDeliveryPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricDeliveryPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFabricDeliveryPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiFabricDeliveryPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_attempts);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->acknowledgement_timeout_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->durable);
}
static void UmiFabricDeliveryPolicyArchiveRead(UmiArchiveReader *reader, UmiFabricDeliveryPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->mode = (UmiFabricDeliveryMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->max_attempts = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->acknowledgement_timeout_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->durable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricDeliveryPolicyArchiveValidate(const UmiFabricDeliveryPolicy *value)
{
    return umi_fabric_delivery_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_delivery_policy_archive_encode, umi_fabric_delivery_policy_archive_decode,
    UmiFabricDeliveryPolicy, UmiFabricDeliveryPolicyArchiveSchema, UmiFabricDeliveryPolicyArchiveBound, UmiFabricDeliveryPolicyArchiveWrite, UmiFabricDeliveryPolicyArchiveRead, UmiFabricDeliveryPolicyArchiveValidate)
