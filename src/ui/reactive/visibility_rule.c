/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/visibility_rule.c
 *
 * PURPOSE:
 *   Bind component visibility to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/visibility_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the visibility rule contract to deterministic zero/default state. */
void umi_ui_reactive_visibility_rule_init(UmiUiReactiveVisibilityRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_visibility_rule_valid(const UmiUiReactiveVisibilityRule *item) {
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
static uint64_t UmiUiReactiveVisibilityRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x878cebf97a121660);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveVisibilityRule *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveVisibilityRule *)0)->expression)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveVisibilityRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveVisibilityRule *)0)->target_id) - 1U +
        8U + sizeof(((UmiUiReactiveVisibilityRule *)0)->expression) - 1U +
        8U;
}
static void UmiUiReactiveVisibilityRuleArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveVisibilityRule *value)
{
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->expression, sizeof(value->expression));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
}
static void UmiUiReactiveVisibilityRuleArchiveRead(UmiArchiveReader *reader, UmiUiReactiveVisibilityRule *value)
{
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->expression, sizeof(value->expression));
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveVisibilityRuleArchiveValidate(const UmiUiReactiveVisibilityRule *value)
{
    return umi_ui_reactive_visibility_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_visibility_rule_archive_encode, umi_ui_reactive_visibility_rule_archive_decode,
    UmiUiReactiveVisibilityRule, UmiUiReactiveVisibilityRuleArchiveSchema, UmiUiReactiveVisibilityRuleArchiveBound, UmiUiReactiveVisibilityRuleArchiveWrite, UmiUiReactiveVisibilityRuleArchiveRead, UmiUiReactiveVisibilityRuleArchiveValidate)
