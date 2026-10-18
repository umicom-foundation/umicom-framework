/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/dead_letter_policy.c
 *
 * PURPOSE:
 *   Describe bounded dead-letter escalation and retention rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/integration/fabric/dead_letter_policy.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric dead letter policy from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_dead_letter_policy_init(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination,sizeof(item->destination),destination);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->after_attempts=after_attempts;item->retention_ms=retention_ms;item->include_payload=include_payload;
    return umi_fabric_dead_letter_policy_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_dead_letter_policy_init_build_value(UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->policy_id,sizeof(item->policy_id),policy_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination,sizeof(item->destination),destination);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->after_attempts=after_attempts;item->retention_ms=retention_ms;item->include_payload=include_payload;
    return umi_fabric_dead_letter_policy_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_dead_letter_policy_init, UmiFabricDeadLetterPolicy, item,
    (UmiFabricDeadLetterPolicy *item, const char *policy_id, const char *destination, uint32_t after_attempts, uint64_t retention_ms, bool include_payload),
    umi_fabric_dead_letter_policy_init_build_value(candidate, policy_id, destination, after_attempts, retention_ms, include_payload))

/*
 * Check that fabric dead letter policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_dead_letter_policy_validate(const UmiFabricDeadLetterPolicy *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->policy_id, '\0', sizeof(item->policy_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->destination, '\0', sizeof(item->destination)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->policy_id[0]!='\0' && item->destination[0]!='\0' && item->after_attempts>0U && item->retention_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricDeadLetterPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa4db86c6e9dbc5f5);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricDeadLetterPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricDeadLetterPolicy *)0)->destination)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricDeadLetterPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricDeadLetterPolicy *)0)->policy_id) - 1U +
        8U + sizeof(((UmiFabricDeadLetterPolicy *)0)->destination) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricDeadLetterPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiFabricDeadLetterPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteText(writer, value->destination, sizeof(value->destination));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->after_attempts);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->retention_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->include_payload);
}
static void UmiFabricDeadLetterPolicyArchiveRead(UmiArchiveReader *reader, UmiFabricDeadLetterPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    UmiArchiveReadText(reader, value->destination, sizeof(value->destination));
    value->after_attempts = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->retention_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->include_payload = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricDeadLetterPolicyArchiveValidate(const UmiFabricDeadLetterPolicy *value)
{
    return umi_fabric_dead_letter_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_dead_letter_policy_archive_encode, umi_fabric_dead_letter_policy_archive_decode,
    UmiFabricDeadLetterPolicy, UmiFabricDeadLetterPolicyArchiveSchema, UmiFabricDeadLetterPolicyArchiveBound, UmiFabricDeadLetterPolicyArchiveWrite, UmiFabricDeadLetterPolicyArchiveRead, UmiFabricDeadLetterPolicyArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_fabric_dead_letter_policy_archive_decode_with_workspace, UmiFabricDeadLetterPolicy,
    UmiFabricDeadLetterPolicyArchiveSchema, UmiFabricDeadLetterPolicyArchiveBound, UmiFabricDeadLetterPolicyArchiveRead, UmiFabricDeadLetterPolicyArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_fabric_dead_letter_policy_archive_decode_if_unchanged, umi_fabric_dead_letter_policy_archive_encode,
    UmiFabricDeadLetterPolicy, UmiFabricDeadLetterPolicyArchiveSchema, UmiFabricDeadLetterPolicyArchiveBound, UmiFabricDeadLetterPolicyArchiveRead, UmiFabricDeadLetterPolicyArchiveValidate)
