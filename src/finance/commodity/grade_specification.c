/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/grade_specification.c
 *
 * PURPOSE:
 *   Implement a named quality grade tied to a canonical commodity.
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

#include "umicom/finance/commodity/grade_specification.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_grade_specification_init(UmiCommodityGradeSpecification *value, const char *id, const char *commodity_id, const char *grade_code)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->grade_code, sizeof value->grade_code, grade_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_grade_specification_valid(const UmiCommodityGradeSpecification *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->commodity_id.value, '\0', sizeof(value->commodity_id.value)) == NULL) return 0;
    if (memchr(value->grade_code, '\0', sizeof(value->grade_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->commodity_id.value) && umi_commodity_text_valid(value->grade_code) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityGradeSpecificationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1f8bd4434fbf50b0);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityGradeSpecification *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityGradeSpecification *)0)->commodity_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityGradeSpecification *)0)->grade_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityGradeSpecificationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityGradeSpecification *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityGradeSpecification *)0)->commodity_id.value) - 1U +
        8U + sizeof(((UmiCommodityGradeSpecification *)0)->grade_code) - 1U +
        8U;
}
static void UmiCommodityGradeSpecificationArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityGradeSpecification *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->commodity_id.value, sizeof(value->commodity_id.value));
    UmiArchiveWriteText(writer, value->grade_code, sizeof(value->grade_code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityGradeSpecificationArchiveRead(UmiArchiveReader *reader, UmiCommodityGradeSpecification *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->commodity_id.value, sizeof(value->commodity_id.value));
    UmiArchiveReadText(reader, value->grade_code, sizeof(value->grade_code));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityGradeSpecificationArchiveValidate(const UmiCommodityGradeSpecification *value)
{
    return umi_commodity_grade_specification_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_grade_specification_archive_encode, umi_commodity_grade_specification_archive_decode,
    UmiCommodityGradeSpecification, UmiCommodityGradeSpecificationArchiveSchema, UmiCommodityGradeSpecificationArchiveBound, UmiCommodityGradeSpecificationArchiveWrite, UmiCommodityGradeSpecificationArchiveRead, UmiCommodityGradeSpecificationArchiveValidate)
