/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/chart_annotation.c
 *
 * PURPOSE:
 *   Describe semantic chart annotations independent of renderer markup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_annotation.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics chart annotation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_chart_annotation_init(UmiAnalyticsChartAnnotation *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->id,sizeof item->id,"annotation");(void)umi_analytics_copy_text(item->text,sizeof item->text,"Annotation");return UMI_STATUS_OK;}
/*
 * Check that analytics chart annotation satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_chart_annotation_valid(const UmiAnalyticsChartAnnotation *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->id, '\0', sizeof(item->id)) == NULL) return 0;
    if (memchr(item->text, '\0', sizeof(item->text)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->id[0]!='\0'&&umi_analytics_number_valid(item->x)&&umi_analytics_number_valid(item->y))?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsChartAnnotationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xaf26c15527185799);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsChartAnnotation *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsChartAnnotation *)0)->text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsChartAnnotationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsChartAnnotation *)0)->id) - 1U +
        8U + sizeof(((UmiAnalyticsChartAnnotation *)0)->text) - 1U +
        8U +
        8U;
}
static void UmiAnalyticsChartAnnotationArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsChartAnnotation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteDouble(writer, value->x);
    UmiArchiveWriteDouble(writer, value->y);
}
static void UmiAnalyticsChartAnnotationArchiveRead(UmiArchiveReader *reader, UmiAnalyticsChartAnnotation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    value->x = UmiArchiveReadDouble(reader);
    value->y = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAnalyticsChartAnnotationArchiveValidate(const UmiAnalyticsChartAnnotation *value)
{
    return umi_analytics_chart_annotation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_chart_annotation_archive_encode, umi_analytics_chart_annotation_archive_decode,
    UmiAnalyticsChartAnnotation, UmiAnalyticsChartAnnotationArchiveSchema, UmiAnalyticsChartAnnotationArchiveBound, UmiAnalyticsChartAnnotationArchiveWrite, UmiAnalyticsChartAnnotationArchiveRead, UmiAnalyticsChartAnnotationArchiveValidate)
