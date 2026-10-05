/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/built_in_converters.c
 *
 * PURPOSE:
 *   Provide deterministic scalar conversion helpers used by declarative bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/built_in_converters.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the built in converters contract to deterministic zero/default state. */
void umi_ui_reactive_built_in_converters_init(UmiUiReactiveBuiltInConverters *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_built_in_converters_valid(const UmiUiReactiveBuiltInConverters *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBuiltInConvertersArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6db55f58a0002631);

    return schema;
}
static size_t UmiUiReactiveBuiltInConvertersArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiUiReactiveBuiltInConvertersArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBuiltInConverters *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_lossy_numeric);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->trim_strings);
}
static void UmiUiReactiveBuiltInConvertersArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBuiltInConverters *value)
{
    value->allow_lossy_numeric = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->trim_strings = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBuiltInConvertersArchiveValidate(const UmiUiReactiveBuiltInConverters *value)
{
    return umi_ui_reactive_built_in_converters_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_built_in_converters_archive_encode, umi_ui_reactive_built_in_converters_archive_decode,
    UmiUiReactiveBuiltInConverters, UmiUiReactiveBuiltInConvertersArchiveSchema, UmiUiReactiveBuiltInConvertersArchiveBound, UmiUiReactiveBuiltInConvertersArchiveWrite, UmiUiReactiveBuiltInConvertersArchiveRead, UmiUiReactiveBuiltInConvertersArchiveValidate)
