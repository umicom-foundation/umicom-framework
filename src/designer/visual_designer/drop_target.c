/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/drop_target.c
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drop_target.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer drop target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "drop_target");
    (void)umi_rad_copy_text(item->slot_id, sizeof item->slot_id, "drop_target");
    item->accepted = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer drop target satisfies its contract before another service relies on it. */
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->parent_id, '\0', sizeof(item->parent_id)) == NULL) return 0;
    if (memchr(item->slot_id, '\0', sizeof(item->slot_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDropTargetArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x79e41e856d00d2f3);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDropTarget *)0)->parent_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDropTarget *)0)->slot_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDropTargetArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDropTarget *)0)->parent_id) - 1U +
        8U + sizeof(((UmiRadDropTarget *)0)->slot_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDropTargetArchiveWrite(UmiArchiveWriter *writer, const UmiRadDropTarget *value)
{
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteText(writer, value->slot_id, sizeof(value->slot_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accepted);
}
static void UmiRadDropTargetArchiveRead(UmiArchiveReader *reader, UmiRadDropTarget *value)
{
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    UmiArchiveReadText(reader, value->slot_id, sizeof(value->slot_id));
    value->bounds.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->accepted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDropTargetArchiveValidate(const UmiRadDropTarget *value)
{
    return umi_rad_drop_target_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_drop_target_archive_encode, umi_rad_drop_target_archive_decode,
    UmiRadDropTarget, UmiRadDropTargetArchiveSchema, UmiRadDropTargetArchiveBound, UmiRadDropTargetArchiveWrite, UmiRadDropTargetArchiveRead, UmiRadDropTargetArchiveValidate)
