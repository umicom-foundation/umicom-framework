/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/crosshair.c
 *
 * PURPOSE:
 *   Track semantic crosshair position, visibility and lock state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/crosshair.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics crosshair from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_crosshair_init(UmiAnalyticsCrosshair *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->x=0.0;item->y=0.0;return UMI_STATUS_OK;}
/*
 * Check that analytics crosshair satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_crosshair_valid(const UmiAnalyticsCrosshair *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (umi_analytics_number_valid(item->x)&&umi_analytics_number_valid(item->y))?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsCrosshairArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5f600491c018424);

    return schema;
}
static size_t UmiAnalyticsCrosshairArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsCrosshairArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsCrosshair *value)
{
    UmiArchiveWriteDouble(writer, value->x);
    UmiArchiveWriteDouble(writer, value->y);
    UmiArchiveWriteSigned(writer, (int64_t)value->visible);
    UmiArchiveWriteSigned(writer, (int64_t)value->locked);
}
static void UmiAnalyticsCrosshairArchiveRead(UmiArchiveReader *reader, UmiAnalyticsCrosshair *value)
{
    value->x = UmiArchiveReadDouble(reader);
    value->y = UmiArchiveReadDouble(reader);
    value->visible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->locked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsCrosshairArchiveValidate(const UmiAnalyticsCrosshair *value)
{
    return umi_analytics_crosshair_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_crosshair_archive_encode, umi_analytics_crosshair_archive_decode,
    UmiAnalyticsCrosshair, UmiAnalyticsCrosshairArchiveSchema, UmiAnalyticsCrosshairArchiveBound, UmiAnalyticsCrosshairArchiveWrite, UmiAnalyticsCrosshairArchiveRead, UmiAnalyticsCrosshairArchiveValidate)
