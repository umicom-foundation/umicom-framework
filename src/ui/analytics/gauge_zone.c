/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/gauge_zone.c
 *
 * PURPOSE:
 *   Define ordered gauge threshold zones with semantic severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/gauge_zone.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics gauge zone from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_gauge_zone_init(UmiAnalyticsGaugeZone *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->minimum=0.0;item->maximum=1.0;item->severity=UMI_ANALYTICS_SEVERITY_INFO;return UMI_STATUS_OK;}
/*
 * Check that analytics gauge zone satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_gauge_zone_valid(const UmiAnalyticsGaugeZone *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->minimum<item->maximum&&item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsGaugeZoneArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x31c01bc58fa5b41e);

    return schema;
}
static size_t UmiAnalyticsGaugeZoneArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsGaugeZoneArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsGaugeZone *value)
{
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
}
static void UmiAnalyticsGaugeZoneArchiveRead(UmiArchiveReader *reader, UmiAnalyticsGaugeZone *value)
{
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->severity = (UmiAnalyticsSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsGaugeZoneArchiveValidate(const UmiAnalyticsGaugeZone *value)
{
    return umi_analytics_gauge_zone_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_gauge_zone_archive_encode, umi_analytics_gauge_zone_archive_decode,
    UmiAnalyticsGaugeZone, UmiAnalyticsGaugeZoneArchiveSchema, UmiAnalyticsGaugeZoneArchiveBound, UmiAnalyticsGaugeZoneArchiveWrite, UmiAnalyticsGaugeZoneArchiveRead, UmiAnalyticsGaugeZoneArchiveValidate)
