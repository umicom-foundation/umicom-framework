/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_graph.c
 *
 * PURPOSE:
 *   Implement aggregate dependency/state graph health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_graph.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the state graph contract to deterministic zero/default state. */
void umi_ui_reactive_state_graph_init(UmiUiReactiveStateGraph *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_graph_valid(const UmiUiReactiveStateGraph *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveStateGraphArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa776e6d2bfb83862);

    return schema;
}
static size_t UmiUiReactiveStateGraphArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveStateGraphArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveStateGraph *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->node_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->edge_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->computed_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->acyclic);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiUiReactiveStateGraphArchiveRead(UmiArchiveReader *reader, UmiUiReactiveStateGraph *value)
{
    value->node_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->edge_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->computed_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->acyclic = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveStateGraphArchiveValidate(const UmiUiReactiveStateGraph *value)
{
    return umi_ui_reactive_state_graph_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_state_graph_archive_encode, umi_ui_reactive_state_graph_archive_decode,
    UmiUiReactiveStateGraph, UmiUiReactiveStateGraphArchiveSchema, UmiUiReactiveStateGraphArchiveBound, UmiUiReactiveStateGraphArchiveWrite, UmiUiReactiveStateGraphArchiveRead, UmiUiReactiveStateGraphArchiveValidate)
