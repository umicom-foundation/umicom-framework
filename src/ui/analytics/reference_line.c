/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/reference_line.c
 *
 * PURPOSE:
 *   Describe labelled horizontal or vertical analytical reference lines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/reference_line.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics reference line from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_reference_line_init(UmiAnalyticsReferenceLine *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Reference");item->orientation=UMI_ANALYTICS_HORIZONTAL;return UMI_STATUS_OK;}
/*
 * Check that analytics reference line satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_reference_line_valid(const UmiAnalyticsReferenceLine *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->label, '\0', sizeof(item->label)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (umi_analytics_number_valid(item->value)&&(item->orientation==UMI_ANALYTICS_HORIZONTAL||item->orientation==UMI_ANALYTICS_VERTICAL))?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsReferenceLineArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc08d54eea790b9f1);
    schema = (schema ^ (uint64_t)sizeof(((UmiAnalyticsReferenceLine *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAnalyticsReferenceLineArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAnalyticsReferenceLine *)0)->label) - 1U +
        8U +
        8U;
}
static void UmiAnalyticsReferenceLineArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsReferenceLine *value)
{
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteDouble(writer, value->value);
}
static void UmiAnalyticsReferenceLineArchiveRead(UmiArchiveReader *reader, UmiAnalyticsReferenceLine *value)
{
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->orientation = (UmiAnalyticsOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAnalyticsReferenceLineArchiveValidate(const UmiAnalyticsReferenceLine *value)
{
    return umi_analytics_reference_line_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_reference_line_archive_encode, umi_analytics_reference_line_archive_decode,
    UmiAnalyticsReferenceLine, UmiAnalyticsReferenceLineArchiveSchema, UmiAnalyticsReferenceLineArchiveBound, UmiAnalyticsReferenceLineArchiveWrite, UmiAnalyticsReferenceLineArchiveRead, UmiAnalyticsReferenceLineArchiveValidate)
