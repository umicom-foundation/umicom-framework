/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/linear_gauge.c
 *
 * PURPOSE:
 *   Configure horizontal/vertical linear gauge presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/linear_gauge.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics linear gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_linear_gauge_init(UmiAnalyticsLinearGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->orientation=UMI_ANALYTICS_HORIZONTAL;return UMI_STATUS_OK;}
/*
 * Check that analytics linear gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_linear_gauge_valid(const UmiAnalyticsLinearGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->orientation==UMI_ANALYTICS_HORIZONTAL||item->orientation==UMI_ANALYTICS_VERTICAL)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsLinearGaugeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc570abfd663c989d);

    return schema;
}
static size_t UmiAnalyticsLinearGaugeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiAnalyticsLinearGaugeArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsLinearGauge *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->reversed);
}
static void UmiAnalyticsLinearGaugeArchiveRead(UmiArchiveReader *reader, UmiAnalyticsLinearGauge *value)
{
    value->orientation = (UmiAnalyticsOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->reversed = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsLinearGaugeArchiveValidate(const UmiAnalyticsLinearGauge *value)
{
    return umi_analytics_linear_gauge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_linear_gauge_archive_encode, umi_analytics_linear_gauge_archive_decode,
    UmiAnalyticsLinearGauge, UmiAnalyticsLinearGaugeArchiveSchema, UmiAnalyticsLinearGaugeArchiveBound, UmiAnalyticsLinearGaugeArchiveWrite, UmiAnalyticsLinearGaugeArchiveRead, UmiAnalyticsLinearGaugeArchiveValidate)
