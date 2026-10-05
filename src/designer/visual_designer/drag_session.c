/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/drag_session.c
 *
 * PURPOSE:
 *   Track a visual component drag operation from press through commit/cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drag_session.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer drag session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "drag_session");
    return UMI_STATUS_OK;
}
/* Check that visual designer drag session satisfies its contract before another service relies on it. */
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDragSessionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x24f53d05d97077a9);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDragSession *)0)->component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDragSessionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDragSession *)0)->component_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDragSessionArchiveWrite(UmiArchiveWriter *writer, const UmiRadDragSession *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->start.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->start.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->current.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->current.y);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiRadDragSessionArchiveRead(UmiArchiveReader *reader, UmiRadDragSession *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    value->start.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->start.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->current.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->current.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDragSessionArchiveValidate(const UmiRadDragSession *value)
{
    return umi_rad_drag_session_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_drag_session_archive_encode, umi_rad_drag_session_archive_decode,
    UmiRadDragSession, UmiRadDragSessionArchiveSchema, UmiRadDragSessionArchiveBound, UmiRadDragSessionArchiveWrite, UmiRadDragSessionArchiveRead, UmiRadDragSessionArchiveValidate)
