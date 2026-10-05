/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/required_rule.c
 *
 * PURPOSE:
 *   Bind form required-state to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/required_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the required rule contract to deterministic zero/default state. */
void umi_ui_reactive_required_rule_init(UmiUiReactiveRequiredRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_required_rule_valid(const UmiUiReactiveRequiredRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->target_id, '\0', sizeof(item->target_id)) == NULL) return 0;
    if (memchr(item->expression, '\0', sizeof(item->expression)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveRequiredRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3e969a4a99241908);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveRequiredRule *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveRequiredRule *)0)->expression)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveRequiredRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveRequiredRule *)0)->target_id) - 1U +
        8U + sizeof(((UmiUiReactiveRequiredRule *)0)->expression) - 1U +
        8U;
}
static void UmiUiReactiveRequiredRuleArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveRequiredRule *value)
{
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->expression, sizeof(value->expression));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
}
static void UmiUiReactiveRequiredRuleArchiveRead(UmiArchiveReader *reader, UmiUiReactiveRequiredRule *value)
{
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->expression, sizeof(value->expression));
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveRequiredRuleArchiveValidate(const UmiUiReactiveRequiredRule *value)
{
    return umi_ui_reactive_required_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_required_rule_archive_encode, umi_ui_reactive_required_rule_archive_decode,
    UmiUiReactiveRequiredRule, UmiUiReactiveRequiredRuleArchiveSchema, UmiUiReactiveRequiredRuleArchiveBound, UmiUiReactiveRequiredRuleArchiveWrite, UmiUiReactiveRequiredRuleArchiveRead, UmiUiReactiveRequiredRuleArchiveValidate)
