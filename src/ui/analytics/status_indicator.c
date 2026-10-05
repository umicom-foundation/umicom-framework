/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/status_indicator.c
 *
 * PURPOSE:
 *   Represent compact semantic status indicators for dashboards and workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/status_indicator.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics status indicator from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_status_indicator_init(UmiAnalyticsStatusIndicator *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Status");item->severity=UMI_ANALYTICS_SEVERITY_INFO;item->active=1;return UMI_STATUS_OK;}
/*
 * Check that analytics status indicator satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_status_indicator_valid(const UmiAnalyticsStatusIndicator *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->label, '\0', sizeof(item->label)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsStatusIndicatorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8de276deb1e7bd58);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsStatusIndicator *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsStatusIndicatorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsStatusIndicator *)0)->label) - 1U +
        8U +
        8U;
}
static void UmiAnalyticsStatusIndicatorArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsStatusIndicator *value)
{
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteSigned(writer, (int64_t)value->active);
}
static void UmiAnalyticsStatusIndicatorArchiveRead(UmiArchiveReader *reader, UmiAnalyticsStatusIndicator *value)
{
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->severity = (UmiAnalyticsSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->active = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsStatusIndicatorArchiveValidate(const UmiAnalyticsStatusIndicator *value)
{
    return umi_analytics_status_indicator_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_status_indicator_archive_encode, umi_analytics_status_indicator_archive_decode,
    UmiAnalyticsStatusIndicator, UmiAnalyticsStatusIndicatorArchiveSchema, UmiAnalyticsStatusIndicatorArchiveBound, UmiAnalyticsStatusIndicatorArchiveWrite, UmiAnalyticsStatusIndicatorArchiveRead, UmiAnalyticsStatusIndicatorArchiveValidate)
