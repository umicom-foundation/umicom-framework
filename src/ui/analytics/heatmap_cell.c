/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/heatmap_cell.c
 *
 * PURPOSE:
 *   Describe one labelled heatmap cell for tooltip and accessibility projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_cell.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics heatmap cell from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_heatmap_cell_init(UmiAnalyticsHeatmapCell *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Cell");return UMI_STATUS_OK;}
/*
 * Check that analytics heatmap cell satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_heatmap_cell_valid(const UmiAnalyticsHeatmapCell *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->label, '\0', sizeof(item->label)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (umi_analytics_number_valid(item->value))?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsHeatmapCellArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x81f6f4daff5e6929);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsHeatmapCell *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsHeatmapCellArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiAnalyticsHeatmapCell *)0)->label) - 1U;
}
static void UmiAnalyticsHeatmapCellArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsHeatmapCell *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
}
static void UmiAnalyticsHeatmapCellArchiveRead(UmiArchiveReader *reader, UmiAnalyticsHeatmapCell *value)
{
    value->row = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->column = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
}
static UmiStatus UmiAnalyticsHeatmapCellArchiveValidate(const UmiAnalyticsHeatmapCell *value)
{
    return umi_analytics_heatmap_cell_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_heatmap_cell_archive_encode, umi_analytics_heatmap_cell_archive_decode,
    UmiAnalyticsHeatmapCell, UmiAnalyticsHeatmapCellArchiveSchema, UmiAnalyticsHeatmapCellArchiveBound, UmiAnalyticsHeatmapCellArchiveWrite, UmiAnalyticsHeatmapCellArchiveRead, UmiAnalyticsHeatmapCellArchiveValidate)
