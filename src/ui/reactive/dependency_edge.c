/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/dependency_edge.c
 *
 * PURPOSE:
 *   Implement a directed reactive dependency edge.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_edge.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the dependency edge contract to deterministic zero/default state. */
void umi_ui_reactive_dependency_edge_init(UmiUiReactiveDependencyEdge *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_dependency_edge_valid(const UmiUiReactiveDependencyEdge *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->from_id, '\0', sizeof(item->from_id)) == NULL) return 0;
    if (memchr(item->to_id, '\0', sizeof(item->to_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveDependencyEdgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9454e9e3b458a691);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveDependencyEdge *)0)->from_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveDependencyEdge *)0)->to_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveDependencyEdgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveDependencyEdge *)0)->from_id) - 1U +
        8U + sizeof(((UmiUiReactiveDependencyEdge *)0)->to_id) - 1U;
}
static void UmiUiReactiveDependencyEdgeArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveDependencyEdge *value)
{
    UmiArchiveWriteText(writer, value->from_id, sizeof(value->from_id));
    UmiArchiveWriteText(writer, value->to_id, sizeof(value->to_id));
}
static void UmiUiReactiveDependencyEdgeArchiveRead(UmiArchiveReader *reader, UmiUiReactiveDependencyEdge *value)
{
    UmiArchiveReadText(reader, value->from_id, sizeof(value->from_id));
    UmiArchiveReadText(reader, value->to_id, sizeof(value->to_id));
}
static UmiStatus UmiUiReactiveDependencyEdgeArchiveValidate(const UmiUiReactiveDependencyEdge *value)
{
    return umi_ui_reactive_dependency_edge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_dependency_edge_archive_encode, umi_ui_reactive_dependency_edge_archive_decode,
    UmiUiReactiveDependencyEdge, UmiUiReactiveDependencyEdgeArchiveSchema, UmiUiReactiveDependencyEdgeArchiveBound, UmiUiReactiveDependencyEdgeArchiveWrite, UmiUiReactiveDependencyEdgeArchiveRead, UmiUiReactiveDependencyEdgeArchiveValidate)
