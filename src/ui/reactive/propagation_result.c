/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/propagation_result.c
 *
 * PURPOSE:
 *   Summarise propagated, skipped and failed binding operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_result.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the propagation result contract to deterministic zero/default state. */
void umi_ui_reactive_propagation_result_init(UmiUiReactivePropagationResult *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_propagation_result_valid(const UmiUiReactivePropagationResult *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactivePropagationResultArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf1f5a4e2ebe097ca);

    return schema;
}
static size_t UmiUiReactivePropagationResultArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactivePropagationResultArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactivePropagationResult *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->propagated);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->skipped);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
}
static void UmiUiReactivePropagationResultArchiveRead(UmiArchiveReader *reader, UmiUiReactivePropagationResult *value)
{
    value->propagated = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->skipped = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->failed = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactivePropagationResultArchiveValidate(const UmiUiReactivePropagationResult *value)
{
    return umi_ui_reactive_propagation_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_propagation_result_archive_encode, umi_ui_reactive_propagation_result_archive_decode,
    UmiUiReactivePropagationResult, UmiUiReactivePropagationResultArchiveSchema, UmiUiReactivePropagationResultArchiveBound, UmiUiReactivePropagationResultArchiveWrite, UmiUiReactivePropagationResultArchiveRead, UmiUiReactivePropagationResultArchiveValidate)
