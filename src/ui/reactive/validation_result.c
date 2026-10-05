/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_result.c
 *
 * PURPOSE:
 *   Implement deterministic validation outcome and user-facing message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_result.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the validation result contract to deterministic zero/default state. */
void umi_ui_reactive_validation_result_init(UmiUiReactiveValidationResult *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_result_valid(const UmiUiReactiveValidationResult *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->rule_id, '\0', sizeof(item->rule_id)) == NULL) return 0;
    if (memchr(item->message, '\0', sizeof(item->message)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveValidationResultArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0a3cde0792da33d1);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveValidationResult *)0)->rule_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveValidationResult *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveValidationResultArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveValidationResult *)0)->rule_id) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveValidationResult *)0)->message) - 1U;
}
static void UmiUiReactiveValidationResultArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveValidationResult *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->valid);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
}
static void UmiUiReactiveValidationResultArchiveRead(UmiArchiveReader *reader, UmiUiReactiveValidationResult *value)
{
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    value->valid = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->severity = (UmiUiReactiveValidationSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
}
static UmiStatus UmiUiReactiveValidationResultArchiveValidate(const UmiUiReactiveValidationResult *value)
{
    return umi_ui_reactive_validation_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_validation_result_archive_encode, umi_ui_reactive_validation_result_archive_decode,
    UmiUiReactiveValidationResult, UmiUiReactiveValidationResultArchiveSchema, UmiUiReactiveValidationResultArchiveBound, UmiUiReactiveValidationResultArchiveWrite, UmiUiReactiveValidationResultArchiveRead, UmiUiReactiveValidationResultArchiveValidate)
