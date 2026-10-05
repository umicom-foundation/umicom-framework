/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_snapshot.c
 *
 * PURPOSE:
 *   Represent immutable dashboard render/data snapshot metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_snapshot.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics dashboard snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_snapshot_init(UmiAnalyticsDashboardSnapshot *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->dashboard_id,sizeof item->dashboard_id,"dashboard");item->revision=1U;item->healthy=1;return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_snapshot_valid(const UmiAnalyticsDashboardSnapshot *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->dashboard_id, '\0', sizeof(item->dashboard_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->dashboard_id[0]!='\0'&&item->revision>0U)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsDashboardSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0fe56936fda39edd);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardSnapshot *)0)->dashboard_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsDashboardSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsDashboardSnapshot *)0)->dashboard_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsDashboardSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsDashboardSnapshot *value)
{
    UmiArchiveWriteText(writer, value->dashboard_id, sizeof(value->dashboard_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->generated_at_ns);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->metric_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->healthy);
}
static void UmiAnalyticsDashboardSnapshotArchiveRead(UmiArchiveReader *reader, UmiAnalyticsDashboardSnapshot *value)
{
    UmiArchiveReadText(reader, value->dashboard_id, sizeof(value->dashboard_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->generated_at_ns = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->metric_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->healthy = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsDashboardSnapshotArchiveValidate(const UmiAnalyticsDashboardSnapshot *value)
{
    return umi_analytics_dashboard_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_dashboard_snapshot_archive_encode, umi_analytics_dashboard_snapshot_archive_decode,
    UmiAnalyticsDashboardSnapshot, UmiAnalyticsDashboardSnapshotArchiveSchema, UmiAnalyticsDashboardSnapshotArchiveBound, UmiAnalyticsDashboardSnapshotArchiveWrite, UmiAnalyticsDashboardSnapshotArchiveRead, UmiAnalyticsDashboardSnapshotArchiveValidate)
