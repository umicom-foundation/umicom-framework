/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/threshold_band.c
 *
 * PURPOSE:
 *   Describe semantic threshold bands for risk, limits and operational analytics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/threshold_band.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics threshold band from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_threshold_band_init(UmiAnalyticsThresholdBand *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->lower=0.0;item->upper=1.0;item->severity=UMI_ANALYTICS_SEVERITY_WARNING;return UMI_STATUS_OK;}
/*
 * Check that analytics threshold band satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_threshold_band_valid(const UmiAnalyticsThresholdBand *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->lower<=item->upper&&item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsThresholdBandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4196a00d5fead20f);

    return schema;
}
static size_t UmiAnalyticsThresholdBandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsThresholdBandArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsThresholdBand *value)
{
    UmiArchiveWriteDouble(writer, value->lower);
    UmiArchiveWriteDouble(writer, value->upper);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
}
static void UmiAnalyticsThresholdBandArchiveRead(UmiArchiveReader *reader, UmiAnalyticsThresholdBand *value)
{
    value->lower = UmiArchiveReadDouble(reader);
    value->upper = UmiArchiveReadDouble(reader);
    value->severity = (UmiAnalyticsSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsThresholdBandArchiveValidate(const UmiAnalyticsThresholdBand *value)
{
    return umi_analytics_threshold_band_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_threshold_band_archive_encode, umi_analytics_threshold_band_archive_decode,
    UmiAnalyticsThresholdBand, UmiAnalyticsThresholdBandArchiveSchema, UmiAnalyticsThresholdBandArchiveBound, UmiAnalyticsThresholdBandArchiveWrite, UmiAnalyticsThresholdBandArchiveRead, UmiAnalyticsThresholdBandArchiveValidate)
