/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/hierarchy_node.c
 *
 * PURPOSE:
 *   Represent one node in the designer object hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/hierarchy_node.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer hierarchy node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_hierarchy_node_init(UmiRadHierarchyNode *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "hierarchy_node");
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "hierarchy_node");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer hierarchy node satisfies its contract before another service relies on
 * it.
 */
int umi_rad_hierarchy_node_is_valid(const UmiRadHierarchyNode *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->node_id, '\0', sizeof(item->node_id)) == NULL) return 0;
    if (memchr(item->parent_id, '\0', sizeof(item->parent_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->node_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadHierarchyNodeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfb7d6cf1317252e9);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadHierarchyNode *)0)->node_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadHierarchyNode *)0)->parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadHierarchyNodeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadHierarchyNode *)0)->node_id) - 1U +
        8U + sizeof(((UmiRadHierarchyNode *)0)->parent_id) - 1U +
        8U +
        8U;
}
static void UmiRadHierarchyNodeArchiveWrite(UmiArchiveWriter *writer, const UmiRadHierarchyNode *value)
{
    UmiArchiveWriteText(writer, value->node_id, sizeof(value->node_id));
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->expanded);
}
static void UmiRadHierarchyNodeArchiveRead(UmiArchiveReader *reader, UmiRadHierarchyNode *value)
{
    UmiArchiveReadText(reader, value->node_id, sizeof(value->node_id));
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->expanded = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadHierarchyNodeArchiveValidate(const UmiRadHierarchyNode *value)
{
    return umi_rad_hierarchy_node_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_hierarchy_node_archive_encode, umi_rad_hierarchy_node_archive_decode,
    UmiRadHierarchyNode, UmiRadHierarchyNodeArchiveSchema, UmiRadHierarchyNodeArchiveBound, UmiRadHierarchyNodeArchiveWrite, UmiRadHierarchyNodeArchiveRead, UmiRadHierarchyNodeArchiveValidate)
