/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis.c
 *
 * PURPOSE:
 *   Describe one semantic chart axis without renderer-specific coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics axis from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_axis_init(UmiAnalyticsAxis *axis,const char *id,const char *label,UmiAnalyticsAxisScale scale,double minimum,double maximum){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(axis==NULL||minimum>=maximum||!umi_analytics_number_valid(minimum)||!umi_analytics_number_valid(maximum))return UMI_STATUS_INVALID_ARGUMENT;memset(axis,0,sizeof *axis);s=umi_analytics_copy_text(axis->id,sizeof axis->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(axis->label,sizeof axis->label,label);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;axis->scale=scale;axis->minimum=minimum;axis->maximum=maximum;return UMI_STATUS_OK;}
/* Check that analytics axis satisfies its contract before another service relies on it. */
int umi_analytics_axis_valid(const UmiAnalyticsAxis *axis){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (axis == NULL) return 0;
    if (memchr(axis->id, '\0', sizeof(axis->id)) == NULL) return 0;
    if (memchr(axis->label, '\0', sizeof(axis->label)) == NULL) return 0;
return axis!=NULL&&axis->id[0]!='\0'&&axis->scale>=UMI_ANALYTICS_SCALE_LINEAR&&axis->scale<=UMI_ANALYTICS_SCALE_CATEGORY&&axis->minimum<axis->maximum;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsAxisArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x14f826017ca8f9b2);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsAxis *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsAxis *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsAxisArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsAxis *)0)->id) - 1U +
        8U + sizeof(((UmiAnalyticsAxis *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsAxisArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsAxis *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->scale);
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteSigned(writer, (int64_t)value->include_zero);
}
static void UmiAnalyticsAxisArchiveRead(UmiArchiveReader *reader, UmiAnalyticsAxis *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->scale = (UmiAnalyticsAxisScale)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->include_zero = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsAxisArchiveValidate(const UmiAnalyticsAxis *value)
{
    return umi_analytics_axis_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_axis_archive_encode, umi_analytics_axis_archive_decode,
    UmiAnalyticsAxis, UmiAnalyticsAxisArchiveSchema, UmiAnalyticsAxisArchiveBound, UmiAnalyticsAxisArchiveWrite, UmiAnalyticsAxisArchiveRead, UmiAnalyticsAxisArchiveValidate)
