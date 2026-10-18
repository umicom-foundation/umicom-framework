/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/aggregate_rule.c
 *
 * PURPOSE:
 *   Describe bounded correlation-based aggregation windows before the canonical messaging aggregator executes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/integration/fabric/aggregate_rule.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric aggregate rule from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_aggregate_rule_init(UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_field,sizeof(item->correlation_field),correlation_field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->expected_count=expected_count;item->timeout_ms=timeout_ms;item->partial_allowed=partial_allowed;
    return umi_fabric_aggregate_rule_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_aggregate_rule_init_build_value(UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_field,sizeof(item->correlation_field),correlation_field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->expected_count=expected_count;item->timeout_ms=timeout_ms;item->partial_allowed=partial_allowed;
    return umi_fabric_aggregate_rule_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_aggregate_rule_init, UmiFabricAggregateRule, item,
    (UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed),
    umi_fabric_aggregate_rule_init_build_value(candidate, rule_id, correlation_field, expected_count, timeout_ms, partial_allowed))

/*
 * Check that fabric aggregate rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_aggregate_rule_validate(const UmiFabricAggregateRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->rule_id, '\0', sizeof(item->rule_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->correlation_field, '\0', sizeof(item->correlation_field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->rule_id[0]!='\0' && item->correlation_field[0]!='\0' && item->expected_count>0U && item->timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricAggregateRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3a6a719194d84137);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricAggregateRule *)0)->rule_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricAggregateRule *)0)->correlation_field)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricAggregateRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricAggregateRule *)0)->rule_id) - 1U +
        8U + sizeof(((UmiFabricAggregateRule *)0)->correlation_field) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricAggregateRuleArchiveWrite(UmiArchiveWriter *writer, const UmiFabricAggregateRule *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteText(writer, value->correlation_field, sizeof(value->correlation_field));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->expected_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timeout_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->partial_allowed);
}
static void UmiFabricAggregateRuleArchiveRead(UmiArchiveReader *reader, UmiFabricAggregateRule *value)
{
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    UmiArchiveReadText(reader, value->correlation_field, sizeof(value->correlation_field));
    value->expected_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->timeout_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->partial_allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricAggregateRuleArchiveValidate(const UmiFabricAggregateRule *value)
{
    return umi_fabric_aggregate_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_aggregate_rule_archive_encode, umi_fabric_aggregate_rule_archive_decode,
    UmiFabricAggregateRule, UmiFabricAggregateRuleArchiveSchema, UmiFabricAggregateRuleArchiveBound, UmiFabricAggregateRuleArchiveWrite, UmiFabricAggregateRuleArchiveRead, UmiFabricAggregateRuleArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_fabric_aggregate_rule_archive_decode_with_workspace, UmiFabricAggregateRule,
    UmiFabricAggregateRuleArchiveSchema, UmiFabricAggregateRuleArchiveBound, UmiFabricAggregateRuleArchiveRead, UmiFabricAggregateRuleArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_fabric_aggregate_rule_archive_decode_if_unchanged, umi_fabric_aggregate_rule_archive_encode,
    UmiFabricAggregateRule, UmiFabricAggregateRuleArchiveSchema, UmiFabricAggregateRuleArchiveBound, UmiFabricAggregateRuleArchiveRead, UmiFabricAggregateRuleArchiveValidate)
