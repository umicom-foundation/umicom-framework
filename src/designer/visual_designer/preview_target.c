/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/preview_target.c
 *
 * PURPOSE:
 *   Describe GTK4, Qt6, Native Web or abstract-device preview targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/preview_target.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer preview target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_target_init(UmiRadPreviewTarget *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "preview_target");
    item->viewport.width = 1280; item->viewport.height = 720;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer preview target satisfies its contract before another service relies on
 * it.
 */
int umi_rad_preview_target_is_valid(const UmiRadPreviewTarget *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->target_id, '\0', sizeof(item->target_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->target_id) && item->viewport.width > 0 && item->viewport.height > 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPreviewTargetArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0828bfa8d3378141);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPreviewTarget *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPreviewTargetArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPreviewTarget *)0)->target_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadPreviewTargetArchiveWrite(UmiArchiveWriter *writer, const UmiRadPreviewTarget *value)
{
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->viewport.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->viewport.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dpi);
}
static void UmiRadPreviewTargetArchiveRead(UmiArchiveReader *reader, UmiRadPreviewTarget *value)
{
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->kind = (UmiRadTargetKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->viewport.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->viewport.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->dpi = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiRadPreviewTargetArchiveValidate(const UmiRadPreviewTarget *value)
{
    return umi_rad_preview_target_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_preview_target_archive_encode, umi_rad_preview_target_archive_decode,
    UmiRadPreviewTarget, UmiRadPreviewTargetArchiveSchema, UmiRadPreviewTargetArchiveBound, UmiRadPreviewTargetArchiveWrite, UmiRadPreviewTargetArchiveRead, UmiRadPreviewTargetArchiveValidate)
