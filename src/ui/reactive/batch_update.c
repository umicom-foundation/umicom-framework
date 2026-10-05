/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/batch_update.c
 *
 * PURPOSE:
 *   Aggregate state mutations into one revision boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/batch_update.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the batch update contract to deterministic zero/default state. */
void umi_ui_reactive_batch_update_init(UmiUiReactiveBatchUpdate *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_batch_update_valid(const UmiUiReactiveBatchUpdate *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBatchUpdateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9a3c2780aab1aa2f);

    return schema;
}
static size_t UmiUiReactiveBatchUpdateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveBatchUpdateArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBatchUpdate *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->mutation_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->committed);
}
static void UmiUiReactiveBatchUpdateArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBatchUpdate *value)
{
    value->mutation_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->start_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->end_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->committed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBatchUpdateArchiveValidate(const UmiUiReactiveBatchUpdate *value)
{
    return umi_ui_reactive_batch_update_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_batch_update_archive_encode, umi_ui_reactive_batch_update_archive_decode,
    UmiUiReactiveBatchUpdate, UmiUiReactiveBatchUpdateArchiveSchema, UmiUiReactiveBatchUpdateArchiveBound, UmiUiReactiveBatchUpdateArchiveWrite, UmiUiReactiveBatchUpdateArchiveRead, UmiUiReactiveBatchUpdateArchiveValidate)
