/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/schedule_rule.c
 *
 * PURPOSE:
 *   Implement schedule rule validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/schedule_rule.h"
#include "../../base/value_archive_internal.h"

/* Validate a schedule rule. */ bool umi_schedule_rule_is_valid(const UmiScheduleRule *r){return r!=NULL&&umi_financial_date_is_valid(r->start)&&umi_financial_date_is_valid(r->end)&&umi_financial_date_compare(r->start,r->end)<0&&r->frequency.amount>0U&&r->frequency.unit<=UMI_TENOR_YEARS&&r->convention<=UMI_BUSINESS_DAY_UNADJUSTED;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiScheduleRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa76aa3686d608a5c);

    return schema;
}
static size_t UmiScheduleRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiScheduleRuleArchiveWrite(UmiArchiveWriter *writer, const UmiScheduleRule *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->start.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start.day);
    UmiArchiveWriteSigned(writer, (int64_t)value->end.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->end.day);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frequency.amount);
    UmiArchiveWriteSigned(writer, (int64_t)value->frequency.unit);
    UmiArchiveWriteSigned(writer, (int64_t)value->convention);
}
static void UmiScheduleRuleArchiveRead(UmiArchiveReader *reader, UmiScheduleRule *value)
{
    value->start.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->start.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->start.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->end.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->end.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->end.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->frequency.amount = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->frequency.unit = (UmiTenorUnit)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->convention = (UmiBusinessDayConvention)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiScheduleRuleArchiveValidate(const UmiScheduleRule *value)
{
    return umi_schedule_rule_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_schedule_rule_archive_encode, umi_schedule_rule_archive_decode,
    UmiScheduleRule, UmiScheduleRuleArchiveSchema, UmiScheduleRuleArchiveBound, UmiScheduleRuleArchiveWrite, UmiScheduleRuleArchiveRead, UmiScheduleRuleArchiveValidate)
