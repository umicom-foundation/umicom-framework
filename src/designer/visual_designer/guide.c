/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/guide.c
 *
 * PURPOSE:
 *   Represent user-created horizontal and vertical design guides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/guide.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer guide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_guide_init(UmiRadGuide *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->guide_id, sizeof item->guide_id, "guide");
    return UMI_STATUS_OK;
}
/* Check that visual designer guide satisfies its contract before another service relies on it. */
int umi_rad_guide_is_valid(const UmiRadGuide *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->guide_id, '\0', sizeof(item->guide_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->guide_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadGuideArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x79fcca1f0b359597);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadGuide *)0)->guide_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadGuideArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadGuide *)0)->guide_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiRadGuideArchiveWrite(UmiArchiveWriter *writer, const UmiRadGuide *value)
{
    UmiArchiveWriteText(writer, value->guide_id, sizeof(value->guide_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->position);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->locked);
}
static void UmiRadGuideArchiveRead(UmiArchiveReader *reader, UmiRadGuide *value)
{
    UmiArchiveReadText(reader, value->guide_id, sizeof(value->guide_id));
    value->orientation = (UmiRadOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->position = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->locked = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadGuideArchiveValidate(const UmiRadGuide *value)
{
    return umi_rad_guide_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_guide_archive_encode, umi_rad_guide_archive_decode,
    UmiRadGuide, UmiRadGuideArchiveSchema, UmiRadGuideArchiveBound, UmiRadGuideArchiveWrite, UmiRadGuideArchiveRead, UmiRadGuideArchiveValidate)
