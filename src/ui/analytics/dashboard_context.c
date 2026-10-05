/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_context.c
 *
 * PURPOSE:
 *   Carry linked entity and time-window context across dashboard tiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_context.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics dashboard context from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_context_init(UmiAnalyticsDashboardContext *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->context_group,sizeof item->context_group,"default");return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard context satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_context_valid(const UmiAnalyticsDashboardContext *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->context_group, '\0', sizeof(item->context_group)) == NULL) return 0;
    if (memchr(item->entity_id, '\0', sizeof(item->entity_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->context_group[0]!='\0'&&item->start_ns<=item->end_ns)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsDashboardContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe5b440991e8639b5);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardContext *)0)->context_group)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsDashboardContext *)0)->entity_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsDashboardContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsDashboardContext *)0)->context_group) - 1U +
        8U + sizeof(((UmiAnalyticsDashboardContext *)0)->entity_id) - 1U +
        8U +
        8U;
}
static void UmiAnalyticsDashboardContextArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsDashboardContext *value)
{
    UmiArchiveWriteText(writer, value->context_group, sizeof(value->context_group));
    UmiArchiveWriteText(writer, value->entity_id, sizeof(value->entity_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->start_ns);
    UmiArchiveWriteSigned(writer, (int64_t)value->end_ns);
}
static void UmiAnalyticsDashboardContextArchiveRead(UmiArchiveReader *reader, UmiAnalyticsDashboardContext *value)
{
    UmiArchiveReadText(reader, value->context_group, sizeof(value->context_group));
    UmiArchiveReadText(reader, value->entity_id, sizeof(value->entity_id));
    value->start_ns = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->end_ns = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiAnalyticsDashboardContextArchiveValidate(const UmiAnalyticsDashboardContext *value)
{
    return umi_analytics_dashboard_context_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_dashboard_context_archive_encode, umi_analytics_dashboard_context_archive_decode,
    UmiAnalyticsDashboardContext, UmiAnalyticsDashboardContextArchiveSchema, UmiAnalyticsDashboardContextArchiveBound, UmiAnalyticsDashboardContextArchiveWrite, UmiAnalyticsDashboardContextArchiveRead, UmiAnalyticsDashboardContextArchiveValidate)
