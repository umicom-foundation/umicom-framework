/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/quality_measure.c
 *
 * PURPOSE:
 *   Implement an inclusive numeric quality requirement such as density or sulphur.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/quality_measure.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_quality_measure_init(UmiCommodityQualityMeasure *value, const char *name, const char *unit_code, int64_t minimum, int64_t maximum, int32_t scale)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || minimum > maximum || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->unit_code, sizeof value->unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->minimum = minimum;
    value->maximum = maximum;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_quality_measure_valid(const UmiCommodityQualityMeasure *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
    if (memchr(value->unit_code, '\0', sizeof(value->unit_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->name) && umi_commodity_text_valid(value->unit_code) && value->minimum <= value->maximum && value->scale >= 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityQualityMeasureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb178a2793c57fb97);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityQualityMeasure *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityQualityMeasure *)0)->unit_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityQualityMeasureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityQualityMeasure *)0)->name) - 1U +
        8U + sizeof(((UmiCommodityQualityMeasure *)0)->unit_code) - 1U +
        8U +
        8U +
        8U;
}
static void UmiCommodityQualityMeasureArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityQualityMeasure *value)
{
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->unit_code, sizeof(value->unit_code));
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum);
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum);
    UmiArchiveWriteSigned(writer, (int64_t)value->scale);
}
static void UmiCommodityQualityMeasureArchiveRead(UmiArchiveReader *reader, UmiCommodityQualityMeasure *value)
{
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->unit_code, sizeof(value->unit_code));
    value->minimum = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->maximum = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiCommodityQualityMeasureArchiveValidate(const UmiCommodityQualityMeasure *value)
{
    return umi_commodity_quality_measure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_quality_measure_archive_encode, umi_commodity_quality_measure_archive_decode,
    UmiCommodityQualityMeasure, UmiCommodityQualityMeasureArchiveSchema, UmiCommodityQualityMeasureArchiveBound, UmiCommodityQualityMeasureArchiveWrite, UmiCommodityQualityMeasureArchiveRead, UmiCommodityQualityMeasureArchiveValidate)
