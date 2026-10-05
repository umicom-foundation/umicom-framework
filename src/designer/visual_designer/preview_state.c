/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/preview_state.c
 *
 * PURPOSE:
 *   Record renderer-neutral preview health and diagnostic counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/preview_state.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer preview state from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_state_init(UmiRadPreviewState *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->document_revision = 0U;
    item->render_revision = 0U;
    item->healthy = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer preview state satisfies its contract before another service relies on it. */
int umi_rad_preview_state_is_valid(const UmiRadPreviewState *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->render_revision <= item->document_revision;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPreviewStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3078e8f66472a252);

    return schema;
}
static size_t UmiRadPreviewStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadPreviewStateArchiveWrite(UmiArchiveWriter *writer, const UmiRadPreviewState *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->document_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->render_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->warning_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->error_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->healthy);
}
static void UmiRadPreviewStateArchiveRead(UmiArchiveReader *reader, UmiRadPreviewState *value)
{
    value->document_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->render_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->warning_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->error_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->healthy = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadPreviewStateArchiveValidate(const UmiRadPreviewState *value)
{
    return umi_rad_preview_state_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_preview_state_archive_encode, umi_rad_preview_state_archive_decode,
    UmiRadPreviewState, UmiRadPreviewStateArchiveSchema, UmiRadPreviewStateArchiveBound, UmiRadPreviewStateArchiveWrite, UmiRadPreviewStateArchiveRead, UmiRadPreviewStateArchiveValidate)
