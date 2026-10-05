/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_rule.c
 *
 * PURPOSE:
 *   Implement common required/range/length validation constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the validation rule contract to deterministic zero/default state. */
void umi_ui_reactive_validation_rule_init(UmiUiReactiveValidationRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_rule_valid(const UmiUiReactiveValidationRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->rule_id, '\0', sizeof(item->rule_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveValidationRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf5093151c323d539);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveValidationRule *)0)->rule_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveValidationRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveValidationRule *)0)->rule_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveValidationRuleArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveValidationRule *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->min_length);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_length);
}
static void UmiUiReactiveValidationRuleArchiveRead(UmiArchiveReader *reader, UmiUiReactiveValidationRule *value)
{
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->min_length = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->max_length = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiUiReactiveValidationRuleArchiveValidate(const UmiUiReactiveValidationRule *value)
{
    return umi_ui_reactive_validation_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_validation_rule_archive_encode, umi_ui_reactive_validation_rule_archive_decode,
    UmiUiReactiveValidationRule, UmiUiReactiveValidationRuleArchiveSchema, UmiUiReactiveValidationRuleArchiveBound, UmiUiReactiveValidationRuleArchiveWrite, UmiUiReactiveValidationRuleArchiveRead, UmiUiReactiveValidationRuleArchiveValidate)
