/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/canvas.c
 *
 * PURPOSE:
 *   Describe a visual application design canvas and its revision state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/canvas.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer canvas from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_canvas_init(UmiRadCanvas *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->document_id, sizeof item->document_id, "canvas");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "canvas");
    return UMI_STATUS_OK;
}
/* Check that visual designer canvas satisfies its contract before another service relies on it. */
int umi_rad_canvas_is_valid(const UmiRadCanvas *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->document_id, '\0', sizeof(item->document_id)) == NULL) return 0;
    if (memchr(item->root_component_id, '\0', sizeof(item->root_component_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->root_component_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadCanvasArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd933a3cf01d9b0db);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadCanvas *)0)->document_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadCanvas *)0)->root_component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadCanvasArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadCanvas *)0)->document_id) - 1U +
        8U + sizeof(((UmiRadCanvas *)0)->root_component_id) - 1U +
        8U +
        8U;
}
static void UmiRadCanvasArchiveWrite(UmiArchiveWriter *writer, const UmiRadCanvas *value)
{
    UmiArchiveWriteText(writer, value->document_id, sizeof(value->document_id));
    UmiArchiveWriteText(writer, value->root_component_id, sizeof(value->root_component_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dirty);
}
static void UmiRadCanvasArchiveRead(UmiArchiveReader *reader, UmiRadCanvas *value)
{
    UmiArchiveReadText(reader, value->document_id, sizeof(value->document_id));
    UmiArchiveReadText(reader, value->root_component_id, sizeof(value->root_component_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->dirty = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadCanvasArchiveValidate(const UmiRadCanvas *value)
{
    return umi_rad_canvas_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_canvas_archive_encode, umi_rad_canvas_archive_decode,
    UmiRadCanvas, UmiRadCanvasArchiveSchema, UmiRadCanvasArchiveBound, UmiRadCanvasArchiveWrite, UmiRadCanvasArchiveRead, UmiRadCanvasArchiveValidate)
