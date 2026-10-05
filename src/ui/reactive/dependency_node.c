/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/dependency_node.c
 *
 * PURPOSE:
 *   Implement one property/computed-state node in the dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_node.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the dependency node contract to deterministic zero/default state. */
void umi_ui_reactive_dependency_node_init(UmiUiReactiveDependencyNode *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_dependency_node_valid(const UmiUiReactiveDependencyNode *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->node_id, '\0', sizeof(item->node_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveDependencyNodeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6dd1759b8ca27020);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveDependencyNode *)0)->node_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveDependencyNodeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveDependencyNode *)0)->node_id) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveDependencyNodeArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveDependencyNode *value)
{
    UmiArchiveWriteText(writer, value->node_id, sizeof(value->node_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->computed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiUiReactiveDependencyNodeArchiveRead(UmiArchiveReader *reader, UmiUiReactiveDependencyNode *value)
{
    UmiArchiveReadText(reader, value->node_id, sizeof(value->node_id));
    value->computed = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveDependencyNodeArchiveValidate(const UmiUiReactiveDependencyNode *value)
{
    return umi_ui_reactive_dependency_node_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_dependency_node_archive_encode, umi_ui_reactive_dependency_node_archive_decode,
    UmiUiReactiveDependencyNode, UmiUiReactiveDependencyNodeArchiveSchema, UmiUiReactiveDependencyNodeArchiveBound, UmiUiReactiveDependencyNodeArchiveWrite, UmiUiReactiveDependencyNodeArchiveRead, UmiUiReactiveDependencyNodeArchiveValidate)
