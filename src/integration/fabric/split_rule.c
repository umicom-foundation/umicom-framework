/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/split_rule.c
 *
 * PURPOSE:
 *   Describe splitter cardinality and empty-part handling independently of payload parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/split_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric split rule from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_split_rule_init(UmiFabricSplitRule *item, const char *rule_id, const char *expression, size_t maximum_parts, bool discard_empty) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->expression,sizeof(item->expression),expression);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->maximum_parts=maximum_parts;item->discard_empty=discard_empty;
    return umi_fabric_split_rule_validate(item);
}
/* Check that fabric split rule satisfies its contract before another service relies on it. */
UmiStatus umi_fabric_split_rule_validate(const UmiFabricSplitRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->rule_id, '\0', sizeof(item->rule_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->expression, '\0', sizeof(item->expression)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->rule_id[0]!='\0' && item->expression[0]!='\0' && item->maximum_parts>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricSplitRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x998d644e98bbd6ef);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricSplitRule *)0)->rule_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricSplitRule *)0)->expression)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricSplitRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricSplitRule *)0)->rule_id) - 1U +
        8U + sizeof(((UmiFabricSplitRule *)0)->expression) - 1U +
        8U +
        8U;
}
static void UmiFabricSplitRuleArchiveWrite(UmiArchiveWriter *writer, const UmiFabricSplitRule *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteText(writer, value->expression, sizeof(value->expression));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_parts);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->discard_empty);
}
static void UmiFabricSplitRuleArchiveRead(UmiArchiveReader *reader, UmiFabricSplitRule *value)
{
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    UmiArchiveReadText(reader, value->expression, sizeof(value->expression));
    value->maximum_parts = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->discard_empty = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricSplitRuleArchiveValidate(const UmiFabricSplitRule *value)
{
    return umi_fabric_split_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_split_rule_archive_encode, umi_fabric_split_rule_archive_decode,
    UmiFabricSplitRule, UmiFabricSplitRuleArchiveSchema, UmiFabricSplitRuleArchiveBound, UmiFabricSplitRuleArchiveWrite, UmiFabricSplitRuleArchiveRead, UmiFabricSplitRuleArchiveValidate)
