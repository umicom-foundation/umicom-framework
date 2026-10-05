/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transform_rule.c
 *
 * PURPOSE:
 *   Describe a deterministic field/content transform without embedding a scripting runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric transform rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_transform_rule_init(UmiFabricTransformRule *item, const char *rule_id, const char *source_path, const char *target_path, const char *operation, bool required) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->source_path,sizeof(item->source_path),source_path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->target_path,sizeof(item->target_path),target_path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->required=required;
    return umi_fabric_transform_rule_validate(item);
}
/*
 * Check that fabric transform rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_transform_rule_validate(const UmiFabricTransformRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->rule_id, '\0', sizeof(item->rule_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_path, '\0', sizeof(item->source_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->target_path, '\0', sizeof(item->target_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation, '\0', sizeof(item->operation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->rule_id[0]!='\0' && item->source_path[0]!='\0' && item->target_path[0]!='\0' && item->operation[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricTransformRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6d159a604ee6c551);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricTransformRule *)0)->rule_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricTransformRule *)0)->source_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricTransformRule *)0)->target_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricTransformRule *)0)->operation)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricTransformRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricTransformRule *)0)->rule_id) - 1U +
        8U + sizeof(((UmiFabricTransformRule *)0)->source_path) - 1U +
        8U + sizeof(((UmiFabricTransformRule *)0)->target_path) - 1U +
        8U + sizeof(((UmiFabricTransformRule *)0)->operation) - 1U +
        8U;
}
static void UmiFabricTransformRuleArchiveWrite(UmiArchiveWriter *writer, const UmiFabricTransformRule *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteText(writer, value->source_path, sizeof(value->source_path));
    UmiArchiveWriteText(writer, value->target_path, sizeof(value->target_path));
    UmiArchiveWriteText(writer, value->operation, sizeof(value->operation));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
}
static void UmiFabricTransformRuleArchiveRead(UmiArchiveReader *reader, UmiFabricTransformRule *value)
{
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    UmiArchiveReadText(reader, value->source_path, sizeof(value->source_path));
    UmiArchiveReadText(reader, value->target_path, sizeof(value->target_path));
    UmiArchiveReadText(reader, value->operation, sizeof(value->operation));
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricTransformRuleArchiveValidate(const UmiFabricTransformRule *value)
{
    return umi_fabric_transform_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_transform_rule_archive_encode, umi_fabric_transform_rule_archive_decode,
    UmiFabricTransformRule, UmiFabricTransformRuleArchiveSchema, UmiFabricTransformRuleArchiveBound, UmiFabricTransformRuleArchiveWrite, UmiFabricTransformRuleArchiveRead, UmiFabricTransformRuleArchiveValidate)
