/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/observable_value.c
 *
 * PURPOSE:
 *   Hold a revisioned value and dirty state for reactive propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_value.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the observable value contract to deterministic zero/default state. */
void umi_ui_reactive_observable_value_init(UmiUiReactiveObservableValue *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_observable_value_valid(const UmiUiReactiveObservableValue *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->value.string_value, '\0', sizeof(item->value.string_value)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveObservableValueArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x67330f818b54c98d);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveObservableValue *)0)->value.string_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveObservableValueArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveObservableValue *)0)->value.string_value) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveObservableValueArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveObservableValue *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->value.kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->value.boolean_value);
    UmiArchiveWriteSigned(writer, (int64_t)value->value.integer_value);
    UmiArchiveWriteDouble(writer, value->value.real_value);
    UmiArchiveWriteText(writer, value->value.string_value, sizeof(value->value.string_value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dirty);
}
static void UmiUiReactiveObservableValueArchiveRead(UmiArchiveReader *reader, UmiUiReactiveObservableValue *value)
{
    value->value.kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value.boolean_value = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value.integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->value.real_value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->value.string_value, sizeof(value->value.string_value));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->dirty = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveObservableValueArchiveValidate(const UmiUiReactiveObservableValue *value)
{
    return umi_ui_reactive_observable_value_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_observable_value_archive_encode, umi_ui_reactive_observable_value_archive_decode,
    UmiUiReactiveObservableValue, UmiUiReactiveObservableValueArchiveSchema, UmiUiReactiveObservableValueArchiveBound, UmiUiReactiveObservableValueArchiveWrite, UmiUiReactiveObservableValueArchiveRead, UmiUiReactiveObservableValueArchiveValidate)
