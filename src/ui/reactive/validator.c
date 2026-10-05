/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validator.c
 *
 * PURPOSE:
 *   Implement a named validator and the value kind it accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the validator contract to deterministic zero/default state. */
void umi_ui_reactive_validator_init(UmiUiReactiveValidator *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validator_valid(const UmiUiReactiveValidator *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->validator_id, '\0', sizeof(item->validator_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveValidatorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdf7fa353b453eb3b);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveValidator *)0)->validator_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveValidatorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveValidator *)0)->validator_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveValidatorArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveValidator *value)
{
    UmiArchiveWriteText(writer, value->validator_id, sizeof(value->validator_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->value_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiUiReactiveValidatorArchiveRead(UmiArchiveReader *reader, UmiUiReactiveValidator *value)
{
    UmiArchiveReadText(reader, value->validator_id, sizeof(value->validator_id));
    value->value_kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->severity = (UmiUiReactiveValidationSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveValidatorArchiveValidate(const UmiUiReactiveValidator *value)
{
    return umi_ui_reactive_validator_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_validator_archive_encode, umi_ui_reactive_validator_archive_decode,
    UmiUiReactiveValidator, UmiUiReactiveValidatorArchiveSchema, UmiUiReactiveValidatorArchiveBound, UmiUiReactiveValidatorArchiveWrite, UmiUiReactiveValidatorArchiveRead, UmiUiReactiveValidatorArchiveValidate)
