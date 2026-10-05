/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/unit_of_measure.c
 *
 * PURPOSE:
 *   Implement a physical unit with a conversion factor to its dimension base unit.
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

#include "umicom/finance/commodity/unit_of_measure.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_unit_of_measure_init(UmiCommodityUnitOfMeasure *value, const char *code, const char *dimension, int64_t numerator, int64_t denominator)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || numerator <= 0 || denominator <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->dimension, sizeof value->dimension, dimension);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->numerator = numerator;
    value->denominator = denominator;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_unit_of_measure_valid(const UmiCommodityUnitOfMeasure *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->code, '\0', sizeof(value->code)) == NULL) return 0;
    if (memchr(value->dimension, '\0', sizeof(value->dimension)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->code) && umi_commodity_text_valid(value->dimension) && value->numerator > 0 && value->denominator > 0 && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityUnitOfMeasureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8c6ab24a38f19509);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityUnitOfMeasure *)0)->code)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityUnitOfMeasure *)0)->dimension)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityUnitOfMeasureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityUnitOfMeasure *)0)->code) - 1U +
        8U + sizeof(((UmiCommodityUnitOfMeasure *)0)->dimension) - 1U +
        8U +
        8U +
        8U;
}
static void UmiCommodityUnitOfMeasureArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityUnitOfMeasure *value)
{
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteText(writer, value->dimension, sizeof(value->dimension));
    UmiArchiveWriteSigned(writer, (int64_t)value->numerator);
    UmiArchiveWriteSigned(writer, (int64_t)value->denominator);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityUnitOfMeasureArchiveRead(UmiArchiveReader *reader, UmiCommodityUnitOfMeasure *value)
{
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    UmiArchiveReadText(reader, value->dimension, sizeof(value->dimension));
    value->numerator = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->denominator = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityUnitOfMeasureArchiveValidate(const UmiCommodityUnitOfMeasure *value)
{
    return umi_commodity_unit_of_measure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_unit_of_measure_archive_encode, umi_commodity_unit_of_measure_archive_decode,
    UmiCommodityUnitOfMeasure, UmiCommodityUnitOfMeasureArchiveSchema, UmiCommodityUnitOfMeasureArchiveBound, UmiCommodityUnitOfMeasureArchiveWrite, UmiCommodityUnitOfMeasureArchiveRead, UmiCommodityUnitOfMeasureArchiveValidate)
