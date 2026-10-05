/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_query.c
 *
 * PURPOSE:
 *   Describe provider-neutral analytical queries for dashboard datasets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_query.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics query from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_query_init(UmiAnalyticsQuery *q,const char *dataset,const char *metric){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(q,0,sizeof *q);s=umi_analytics_copy_text(q->dataset_id,sizeof q->dataset_id,dataset);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(q->metric_id,sizeof q->metric_id,metric);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;q->limit=1000U;return UMI_STATUS_OK;}
/* Check that analytics query satisfies its contract before another service relies on it. */
int umi_analytics_query_valid(const UmiAnalyticsQuery *q){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (q == NULL) return 0;
    if (memchr(q->dataset_id, '\0', sizeof(q->dataset_id)) == NULL) return 0;
    if (memchr(q->metric_id, '\0', sizeof(q->metric_id)) == NULL) return 0;
    if (memchr(q->group_by, '\0', sizeof(q->group_by)) == NULL) return 0;
return q!=NULL&&q->dataset_id[0]!='\0'&&q->metric_id[0]!='\0'&&q->start_ns<=q->end_ns&&q->limit>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsQueryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5e56c5f143847b3);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsQuery *)0)->dataset_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsQuery *)0)->metric_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsQuery *)0)->group_by)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsQueryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsQuery *)0)->dataset_id) - 1U +
        8U + sizeof(((UmiAnalyticsQuery *)0)->metric_id) - 1U +
        8U + sizeof(((UmiAnalyticsQuery *)0)->group_by) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsQueryArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsQuery *value)
{
    UmiArchiveWriteText(writer, value->dataset_id, sizeof(value->dataset_id));
    UmiArchiveWriteText(writer, value->metric_id, sizeof(value->metric_id));
    UmiArchiveWriteText(writer, value->group_by, sizeof(value->group_by));
    UmiArchiveWriteSigned(writer, (int64_t)value->start_ns);
    UmiArchiveWriteSigned(writer, (int64_t)value->end_ns);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->limit);
}
static void UmiAnalyticsQueryArchiveRead(UmiArchiveReader *reader, UmiAnalyticsQuery *value)
{
    UmiArchiveReadText(reader, value->dataset_id, sizeof(value->dataset_id));
    UmiArchiveReadText(reader, value->metric_id, sizeof(value->metric_id));
    UmiArchiveReadText(reader, value->group_by, sizeof(value->group_by));
    value->start_ns = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->end_ns = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->limit = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiAnalyticsQueryArchiveValidate(const UmiAnalyticsQuery *value)
{
    return umi_analytics_query_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_query_archive_encode, umi_analytics_query_archive_decode,
    UmiAnalyticsQuery, UmiAnalyticsQueryArchiveSchema, UmiAnalyticsQueryArchiveBound, UmiAnalyticsQueryArchiveWrite, UmiAnalyticsQueryArchiveRead, UmiAnalyticsQueryArchiveValidate)
