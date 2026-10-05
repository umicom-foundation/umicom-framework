/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/radial_gauge.c
 *
 * PURPOSE:
 *   Configure radial-gauge angular sweep and needle visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/radial_gauge.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics radial gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_radial_gauge_init(UmiAnalyticsRadialGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->start_degrees=-135.0;item->sweep_degrees=270.0;item->needle=1;return UMI_STATUS_OK;}
/*
 * Check that analytics radial gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_radial_gauge_valid(const UmiAnalyticsRadialGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->sweep_degrees>0.0&&item->sweep_degrees<=360.0)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsRadialGaugeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb7abcb329d510514);

    return schema;
}
static size_t UmiAnalyticsRadialGaugeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsRadialGaugeArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsRadialGauge *value)
{
    UmiArchiveWriteDouble(writer, value->start_degrees);
    UmiArchiveWriteDouble(writer, value->sweep_degrees);
    UmiArchiveWriteSigned(writer, (int64_t)value->needle);
}
static void UmiAnalyticsRadialGaugeArchiveRead(UmiArchiveReader *reader, UmiAnalyticsRadialGauge *value)
{
    value->start_degrees = UmiArchiveReadDouble(reader);
    value->sweep_degrees = UmiArchiveReadDouble(reader);
    value->needle = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsRadialGaugeArchiveValidate(const UmiAnalyticsRadialGauge *value)
{
    return umi_analytics_radial_gauge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_radial_gauge_archive_encode, umi_analytics_radial_gauge_archive_decode,
    UmiAnalyticsRadialGauge, UmiAnalyticsRadialGaugeArchiveSchema, UmiAnalyticsRadialGaugeArchiveBound, UmiAnalyticsRadialGaugeArchiveWrite, UmiAnalyticsRadialGaugeArchiveRead, UmiAnalyticsRadialGaugeArchiveValidate)
