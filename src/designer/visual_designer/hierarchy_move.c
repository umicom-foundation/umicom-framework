/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/hierarchy_move.c
 *
 * PURPOSE:
 *   Describe a reviewable hierarchy reparent/reorder operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/hierarchy_move.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer hierarchy move from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_hierarchy_move_init(UmiRadHierarchyMove *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->old_parent_id, sizeof item->old_parent_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "hierarchy_move");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer hierarchy move satisfies its contract before another service relies on
 * it.
 */
int umi_rad_hierarchy_move_is_valid(const UmiRadHierarchyMove *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->node_id, '\0', sizeof(item->node_id)) == NULL) return 0;
    if (memchr(item->old_parent_id, '\0', sizeof(item->old_parent_id)) == NULL) return 0;
    if (memchr(item->new_parent_id, '\0', sizeof(item->new_parent_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->node_id) && item->new_order >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadHierarchyMoveArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfbb083bdbb267149);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadHierarchyMove *)0)->node_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadHierarchyMove *)0)->old_parent_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadHierarchyMove *)0)->new_parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadHierarchyMoveArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadHierarchyMove *)0)->node_id) - 1U +
        8U + sizeof(((UmiRadHierarchyMove *)0)->old_parent_id) - 1U +
        8U + sizeof(((UmiRadHierarchyMove *)0)->new_parent_id) - 1U +
        8U;
}
static void UmiRadHierarchyMoveArchiveWrite(UmiArchiveWriter *writer, const UmiRadHierarchyMove *value)
{
    UmiArchiveWriteText(writer, value->node_id, sizeof(value->node_id));
    UmiArchiveWriteText(writer, value->old_parent_id, sizeof(value->old_parent_id));
    UmiArchiveWriteText(writer, value->new_parent_id, sizeof(value->new_parent_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->new_order);
}
static void UmiRadHierarchyMoveArchiveRead(UmiArchiveReader *reader, UmiRadHierarchyMove *value)
{
    UmiArchiveReadText(reader, value->node_id, sizeof(value->node_id));
    UmiArchiveReadText(reader, value->old_parent_id, sizeof(value->old_parent_id));
    UmiArchiveReadText(reader, value->new_parent_id, sizeof(value->new_parent_id));
    value->new_order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadHierarchyMoveArchiveValidate(const UmiRadHierarchyMove *value)
{
    return umi_rad_hierarchy_move_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_hierarchy_move_archive_encode, umi_rad_hierarchy_move_archive_decode,
    UmiRadHierarchyMove, UmiRadHierarchyMoveArchiveSchema, UmiRadHierarchyMoveArchiveBound, UmiRadHierarchyMoveArchiveWrite, UmiRadHierarchyMoveArchiveRead, UmiRadHierarchyMoveArchiveValidate)
