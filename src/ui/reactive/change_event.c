/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_event.c
 *
 * PURPOSE:
 *   Implement one observable property change with monotonic sequence metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_event.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the change event contract to deterministic zero/default state. */
void umi_ui_reactive_change_event_init(UmiUiReactiveChangeEvent *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_change_event_valid(const UmiUiReactiveChangeEvent *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->path, '\0', sizeof(item->path)) == NULL) return 0;
    if (memchr(item->before_value.string_value, '\0', sizeof(item->before_value.string_value)) == NULL) return 0;
    if (memchr(item->after_value.string_value, '\0', sizeof(item->after_value.string_value)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveChangeEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbf8fbfe3918e29fb);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveChangeEvent *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveChangeEvent *)0)->before_value.string_value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveChangeEvent *)0)->after_value.string_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveChangeEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveChangeEvent *)0)->path) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveChangeEvent *)0)->before_value.string_value) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveChangeEvent *)0)->after_value.string_value) - 1U +
        8U;
}
static void UmiUiReactiveChangeEventArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveChangeEvent *value)
{
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteSigned(writer, (int64_t)value->before_value.kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->before_value.boolean_value);
    UmiArchiveWriteSigned(writer, (int64_t)value->before_value.integer_value);
    UmiArchiveWriteDouble(writer, value->before_value.real_value);
    UmiArchiveWriteText(writer, value->before_value.string_value, sizeof(value->before_value.string_value));
    UmiArchiveWriteSigned(writer, (int64_t)value->after_value.kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->after_value.boolean_value);
    UmiArchiveWriteSigned(writer, (int64_t)value->after_value.integer_value);
    UmiArchiveWriteDouble(writer, value->after_value.real_value);
    UmiArchiveWriteText(writer, value->after_value.string_value, sizeof(value->after_value.string_value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiUiReactiveChangeEventArchiveRead(UmiArchiveReader *reader, UmiUiReactiveChangeEvent *value)
{
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    value->before_value.kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->before_value.boolean_value = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->before_value.integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->before_value.real_value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->before_value.string_value, sizeof(value->before_value.string_value));
    value->after_value.kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->after_value.boolean_value = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->after_value.integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->after_value.real_value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->after_value.string_value, sizeof(value->after_value.string_value));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveChangeEventArchiveValidate(const UmiUiReactiveChangeEvent *value)
{
    return umi_ui_reactive_change_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_change_event_archive_encode, umi_ui_reactive_change_event_archive_decode,
    UmiUiReactiveChangeEvent, UmiUiReactiveChangeEventArchiveSchema, UmiUiReactiveChangeEventArchiveBound, UmiUiReactiveChangeEventArchiveWrite, UmiUiReactiveChangeEventArchiveRead, UmiUiReactiveChangeEventArchiveValidate)
