/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/resize_handle.c
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/resize_handle.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer resize handle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->edges = 1U;
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer resize handle satisfies its contract before another service relies on it. */
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->edges != 0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadResizeHandleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x68713a55949f58ec);

    return schema;
}
static size_t UmiRadResizeHandleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadResizeHandleArchiveWrite(UmiArchiveWriter *writer, const UmiRadResizeHandle *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->edges);
    UmiArchiveWriteSigned(writer, (int64_t)value->location.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->location.y);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiRadResizeHandleArchiveRead(UmiArchiveReader *reader, UmiRadResizeHandle *value)
{
    value->edges = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->location.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->location.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadResizeHandleArchiveValidate(const UmiRadResizeHandle *value)
{
    return umi_rad_resize_handle_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_resize_handle_archive_encode, umi_rad_resize_handle_archive_decode,
    UmiRadResizeHandle, UmiRadResizeHandleArchiveSchema, UmiRadResizeHandleArchiveBound, UmiRadResizeHandleArchiveWrite, UmiRadResizeHandleArchiveRead, UmiRadResizeHandleArchiveValidate)
