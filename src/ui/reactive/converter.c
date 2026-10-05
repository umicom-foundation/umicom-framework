/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/converter.c
 *
 * PURPOSE:
 *   Implement a named value converter with forward and reverse availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the converter contract to deterministic zero/default state. */
void umi_ui_reactive_converter_init(UmiUiReactiveConverter *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_converter_valid(const UmiUiReactiveConverter *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->converter_id, '\0', sizeof(item->converter_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveConverterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeabcf0070dea2daf);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveConverter *)0)->converter_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveConverterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveConverter *)0)->converter_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveConverterArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveConverter *value)
{
    UmiArchiveWriteText(writer, value->converter_id, sizeof(value->converter_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->source_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->target_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_reverse);
}
static void UmiUiReactiveConverterArchiveRead(UmiArchiveReader *reader, UmiUiReactiveConverter *value)
{
    UmiArchiveReadText(reader, value->converter_id, sizeof(value->converter_id));
    value->source_kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target_kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->supports_reverse = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveConverterArchiveValidate(const UmiUiReactiveConverter *value)
{
    return umi_ui_reactive_converter_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_converter_archive_encode, umi_ui_reactive_converter_archive_decode,
    UmiUiReactiveConverter, UmiUiReactiveConverterArchiveSchema, UmiUiReactiveConverterArchiveBound, UmiUiReactiveConverterArchiveWrite, UmiUiReactiveConverterArchiveRead, UmiUiReactiveConverterArchiveValidate)
