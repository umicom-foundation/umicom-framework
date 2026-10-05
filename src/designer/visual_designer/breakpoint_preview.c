/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/breakpoint_preview.c
 *
 * PURPOSE:
 *   Resolve a named responsive preview breakpoint for the visual canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/breakpoint_preview.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer breakpoint preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_breakpoint_preview_init(UmiRadBreakpointPreview *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->breakpoint_id, sizeof item->breakpoint_id, "breakpoint_preview");
    item->viewport.width = 1280; item->viewport.height = 720;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer breakpoint preview satisfies its contract before another service relies
 * on it.
 */
int umi_rad_breakpoint_preview_is_valid(const UmiRadBreakpointPreview *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->breakpoint_id, '\0', sizeof(item->breakpoint_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->breakpoint_id) && item->viewport.width > 0 && item->viewport.height > 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadBreakpointPreviewArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6a267b7026456d9f);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadBreakpointPreview *)0)->breakpoint_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadBreakpointPreviewArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadBreakpointPreview *)0)->breakpoint_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadBreakpointPreviewArchiveWrite(UmiArchiveWriter *writer, const UmiRadBreakpointPreview *value)
{
    UmiArchiveWriteText(writer, value->breakpoint_id, sizeof(value->breakpoint_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->viewport.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->viewport.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dpi);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->touch);
}
static void UmiRadBreakpointPreviewArchiveRead(UmiArchiveReader *reader, UmiRadBreakpointPreview *value)
{
    UmiArchiveReadText(reader, value->breakpoint_id, sizeof(value->breakpoint_id));
    value->viewport.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->viewport.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->dpi = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->touch = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadBreakpointPreviewArchiveValidate(const UmiRadBreakpointPreview *value)
{
    return umi_rad_breakpoint_preview_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_breakpoint_preview_archive_encode, umi_rad_breakpoint_preview_archive_decode,
    UmiRadBreakpointPreview, UmiRadBreakpointPreviewArchiveSchema, UmiRadBreakpointPreviewArchiveBound, UmiRadBreakpointPreviewArchiveWrite, UmiRadBreakpointPreviewArchiveRead, UmiRadBreakpointPreviewArchiveValidate)
