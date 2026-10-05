/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_tile.c
 *
 * PURPOSE:
 *   Describe one grid-positioned dashboard tile and its semantic component identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_tile.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics dashboard tile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_tile_init(UmiAnalyticsDashboardTile *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->id,sizeof item->id,"tile");(void)umi_analytics_copy_text(item->component_id,sizeof item->component_id,"component");item->row_span=1U;item->column_span=1U;return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard tile satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_dashboard_tile_valid(const UmiAnalyticsDashboardTile *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->id, '\0', sizeof(item->id)) == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->id[0]!='\0'&&item->component_id[0]!='\0'&&item->row_span>0U&&item->column_span>0U)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsDashboardTileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9366194cf99a4f44);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardTile *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardTile *)0)->component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsDashboardTileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsDashboardTile *)0)->id) - 1U +
        8U + sizeof(((UmiAnalyticsDashboardTile *)0)->component_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsDashboardTileArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsDashboardTile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_span);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column_span);
}
static void UmiAnalyticsDashboardTileArchiveRead(UmiArchiveReader *reader, UmiAnalyticsDashboardTile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    value->row = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->column = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->row_span = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->column_span = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
}
static UmiStatus UmiAnalyticsDashboardTileArchiveValidate(const UmiAnalyticsDashboardTile *value)
{
    return umi_analytics_dashboard_tile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_dashboard_tile_archive_encode, umi_analytics_dashboard_tile_archive_decode,
    UmiAnalyticsDashboardTile, UmiAnalyticsDashboardTileArchiveSchema, UmiAnalyticsDashboardTileArchiveBound, UmiAnalyticsDashboardTileArchiveWrite, UmiAnalyticsDashboardTileArchiveRead, UmiAnalyticsDashboardTileArchiveValidate)
