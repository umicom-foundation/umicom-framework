/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/alignment_guide.c
 *
 * PURPOSE:
 *   Represent alignment evidence between visual components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/alignment_guide.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer alignment guide from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_alignment_guide_init(UmiRadAlignmentGuide *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->source_id, sizeof item->source_id, "alignment_guide");
    (void)umi_rad_copy_text(item->peer_id, sizeof item->peer_id, "alignment_guide");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer alignment guide satisfies its contract before another service relies on
 * it.
 */
int umi_rad_alignment_guide_is_valid(const UmiRadAlignmentGuide *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->source_id, '\0', sizeof(item->source_id)) == NULL) return 0;
    if (memchr(item->peer_id, '\0', sizeof(item->peer_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->source_id) && umi_rad_id_valid(item->peer_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadAlignmentGuideArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdc7d01b8f60f71aa);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadAlignmentGuide *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadAlignmentGuide *)0)->peer_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadAlignmentGuideArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadAlignmentGuide *)0)->source_id) - 1U +
        8U + sizeof(((UmiRadAlignmentGuide *)0)->peer_id) - 1U +
        8U +
        8U;
}
static void UmiRadAlignmentGuideArchiveWrite(UmiArchiveWriter *writer, const UmiRadAlignmentGuide *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->peer_id, sizeof(value->peer_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->position);
}
static void UmiRadAlignmentGuideArchiveRead(UmiArchiveReader *reader, UmiRadAlignmentGuide *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->peer_id, sizeof(value->peer_id));
    value->orientation = (UmiRadOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->position = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadAlignmentGuideArchiveValidate(const UmiRadAlignmentGuide *value)
{
    return umi_rad_alignment_guide_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_alignment_guide_archive_encode, umi_rad_alignment_guide_archive_decode,
    UmiRadAlignmentGuide, UmiRadAlignmentGuideArchiveSchema, UmiRadAlignmentGuideArchiveBound, UmiRadAlignmentGuideArchiveWrite, UmiRadAlignmentGuideArchiveRead, UmiRadAlignmentGuideArchiveValidate)
