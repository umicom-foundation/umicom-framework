/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_group.c
 *
 * PURPOSE:
 *   Aggregate validation results for one form, object or editing transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_group.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the validation group contract to deterministic zero/default state. */
void umi_ui_reactive_validation_group_init(UmiUiReactiveValidationGroup *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_group_valid(const UmiUiReactiveValidationGroup *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveValidationGroupArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3104e48e512fe7d8);

    return schema;
}
static size_t UmiUiReactiveValidationGroupArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveValidationGroupArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveValidationGroup *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->invalid);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->warnings);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->blocking);
}
static void UmiUiReactiveValidationGroupArchiveRead(UmiArchiveReader *reader, UmiUiReactiveValidationGroup *value)
{
    value->total = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->invalid = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->warnings = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->blocking = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveValidationGroupArchiveValidate(const UmiUiReactiveValidationGroup *value)
{
    return umi_ui_reactive_validation_group_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_validation_group_archive_encode, umi_ui_reactive_validation_group_archive_decode,
    UmiUiReactiveValidationGroup, UmiUiReactiveValidationGroupArchiveSchema, UmiUiReactiveValidationGroupArchiveBound, UmiUiReactiveValidationGroupArchiveWrite, UmiUiReactiveValidationGroupArchiveRead, UmiUiReactiveValidationGroupArchiveValidate)
