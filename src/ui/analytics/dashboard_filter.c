/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_filter.c
 *
 * PURPOSE:
 *   Describe one dashboard-level textual filter propagated to compatible tiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_filter.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics dashboard filter from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_filter_init(UmiAnalyticsDashboardFilter *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->key,sizeof item->key,"filter");(void)umi_analytics_copy_text(item->value,sizeof item->value,"all");return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard filter satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_filter_valid(const UmiAnalyticsDashboardFilter *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->key, '\0', sizeof(item->key)) == NULL) return 0;
    if (memchr(item->value, '\0', sizeof(item->value)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->key[0]!='\0')?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsDashboardFilterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb5975c27d1e4a709);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardFilter *)0)->key)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardFilter *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsDashboardFilterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsDashboardFilter *)0)->key) - 1U +
        8U + sizeof(((UmiAnalyticsDashboardFilter *)0)->value) - 1U +
        8U;
}
static void UmiAnalyticsDashboardFilterArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsDashboardFilter *value)
{
    UmiArchiveWriteText(writer, value->key, sizeof(value->key));
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
    UmiArchiveWriteSigned(writer, (int64_t)value->case_sensitive);
}
static void UmiAnalyticsDashboardFilterArchiveRead(UmiArchiveReader *reader, UmiAnalyticsDashboardFilter *value)
{
    UmiArchiveReadText(reader, value->key, sizeof(value->key));
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
    value->case_sensitive = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsDashboardFilterArchiveValidate(const UmiAnalyticsDashboardFilter *value)
{
    return umi_analytics_dashboard_filter_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_dashboard_filter_archive_encode, umi_analytics_dashboard_filter_archive_decode,
    UmiAnalyticsDashboardFilter, UmiAnalyticsDashboardFilterArchiveSchema, UmiAnalyticsDashboardFilterArchiveBound, UmiAnalyticsDashboardFilterArchiveWrite, UmiAnalyticsDashboardFilterArchiveRead, UmiAnalyticsDashboardFilterArchiveValidate)
