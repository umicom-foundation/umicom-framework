/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/canvas_viewport.c
 *
 * PURPOSE:
 *   Track canvas origin, dimensions and zoom independently from document geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/canvas_viewport.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer canvas viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_canvas_viewport_init(UmiRadCanvasViewport *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->extent.width = 1280; item->extent.height = 720;
    item->zoom = 1.0;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer canvas viewport satisfies its contract before another service relies on
 * it.
 */
int umi_rad_canvas_viewport_is_valid(const UmiRadCanvasViewport *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->extent.width > 0 && item->extent.height > 0 && item->zoom > 0.0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadCanvasViewportArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x786ed8d082ae7d6b);

    return schema;
}
static size_t UmiRadCanvasViewportArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadCanvasViewportArchiveWrite(UmiArchiveWriter *writer, const UmiRadCanvasViewport *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->origin.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->origin.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->extent.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->extent.height);
    UmiArchiveWriteDouble(writer, value->zoom);
}
static void UmiRadCanvasViewportArchiveRead(UmiArchiveReader *reader, UmiRadCanvasViewport *value)
{
    value->origin.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->origin.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->extent.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->extent.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->zoom = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiRadCanvasViewportArchiveValidate(const UmiRadCanvasViewport *value)
{
    return umi_rad_canvas_viewport_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_canvas_viewport_archive_encode, umi_rad_canvas_viewport_archive_decode,
    UmiRadCanvasViewport, UmiRadCanvasViewportArchiveSchema, UmiRadCanvasViewportArchiveBound, UmiRadCanvasViewportArchiveWrite, UmiRadCanvasViewportArchiveRead, UmiRadCanvasViewportArchiveValidate)
