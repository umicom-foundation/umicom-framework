/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/resequencing_policy.c
 *
 * PURPOSE:
 *   Describe ordering window and gap handling for out-of-order integration messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/resequencing_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric resequencing policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_resequencing_policy_init(UmiFabricResequencingPolicy *item, const char *policy_id, size_t maximum_buffered, uint64_t gap_timeout_ms, bool release_on_timeout) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->maximum_buffered=maximum_buffered;item->gap_timeout_ms=gap_timeout_ms;item->release_on_timeout=release_on_timeout;
    return umi_fabric_resequencing_policy_validate(item);
}
/*
 * Check that fabric resequencing policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_resequencing_policy_validate(const UmiFabricResequencingPolicy *item) {
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->policy_id[0]!='\0' && item->maximum_buffered>0U && item->gap_timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricResequencingPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xde8a8f39e016d34e);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricResequencingPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricResequencingPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricResequencingPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricResequencingPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiFabricResequencingPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_buffered);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->gap_timeout_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->release_on_timeout);
}
static void UmiFabricResequencingPolicyArchiveRead(UmiArchiveReader *reader, UmiFabricResequencingPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->maximum_buffered = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->gap_timeout_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->release_on_timeout = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricResequencingPolicyArchiveValidate(const UmiFabricResequencingPolicy *value)
{
    return umi_fabric_resequencing_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_resequencing_policy_archive_encode, umi_fabric_resequencing_policy_archive_decode,
    UmiFabricResequencingPolicy, UmiFabricResequencingPolicyArchiveSchema, UmiFabricResequencingPolicyArchiveBound, UmiFabricResequencingPolicyArchiveWrite, UmiFabricResequencingPolicyArchiveRead, UmiFabricResequencingPolicyArchiveValidate)
