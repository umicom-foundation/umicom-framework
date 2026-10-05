/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/distribution_guide.c
 *
 * PURPOSE:
 *   Represent equal-spacing evidence for multiple selected components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/distribution_guide.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer distribution guide from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_distribution_guide_init(UmiRadDistributionGuide *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->group_id, sizeof item->group_id, "distribution_guide");
    item->spacing = 1;
    item->item_count = 2U;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer distribution guide satisfies its contract before another service relies
 * on it.
 */
int umi_rad_distribution_guide_is_valid(const UmiRadDistributionGuide *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->group_id, '\0', sizeof(item->group_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->group_id) && item->spacing >= 0 && item->item_count >= 2U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDistributionGuideArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7f94ed969f664e19);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDistributionGuide *)0)->group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDistributionGuideArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDistributionGuide *)0)->group_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiRadDistributionGuideArchiveWrite(UmiArchiveWriter *writer, const UmiRadDistributionGuide *value)
{
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->spacing);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
}
static void UmiRadDistributionGuideArchiveRead(UmiArchiveReader *reader, UmiRadDistributionGuide *value)
{
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    value->orientation = (UmiRadOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->spacing = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->item_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiRadDistributionGuideArchiveValidate(const UmiRadDistributionGuide *value)
{
    return umi_rad_distribution_guide_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_distribution_guide_archive_encode, umi_rad_distribution_guide_archive_decode,
    UmiRadDistributionGuide, UmiRadDistributionGuideArchiveSchema, UmiRadDistributionGuideArchiveBound, UmiRadDistributionGuideArchiveWrite, UmiRadDistributionGuideArchiveRead, UmiRadDistributionGuideArchiveValidate)
