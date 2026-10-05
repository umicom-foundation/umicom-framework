/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/balance.c
 *
 * PURPOSE:
 *   Implement dated financial balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/balance.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize monetary record. */ UmiStatus umi_balance_init(UmiFinancialBalance *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state){UmiStatus st;if(x==NULL||strlen(amount.currency.code)!=3U||!umi_financial_date_is_valid(date))return UMI_STATUS_INVALID_ARGUMENT;st=umi_financial_id_assign(&x->id,id);if(st!=UMI_STATUS_OK)return st;x->amount=amount;x->date=date;x->state=state;return UMI_STATUS_OK;}
/* Validate monetary record. */ bool umi_balance_is_valid(const UmiFinancialBalance *x){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (x == NULL) return 0;
    if (memchr(x->id.value, '\0', sizeof(x->id.value)) == NULL) return 0;
    if (memchr(x->amount.currency.code, '\0', sizeof(x->amount.currency.code)) == NULL) return 0;
return x!=NULL&&umi_financial_id_is_valid(&x->id)&&strlen(x->amount.currency.code)==3U&&umi_financial_date_is_valid(x->date);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialBalanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa215461c0ecdc590);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialBalance *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialBalance *)0)->amount.currency.code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialBalanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialBalance *)0)->id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiFinancialBalance *)0)->amount.currency.code) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFinancialBalanceArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialBalance *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount.minor_units);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->amount.scale);
    UmiArchiveWriteText(writer, value->amount.currency.code, sizeof(value->amount.currency.code));
    UmiArchiveWriteSigned(writer, (int64_t)value->date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->date.day);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state);
}
static void UmiFinancialBalanceArchiveRead(UmiArchiveReader *reader, UmiFinancialBalance *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->amount.minor_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->amount.scale = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    UmiArchiveReadText(reader, value->amount.currency.code, sizeof(value->amount.currency.code));
    value->date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiFinancialBalanceArchiveValidate(const UmiFinancialBalance *value)
{
    return umi_balance_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_balance_archive_encode, umi_balance_archive_decode,
    UmiFinancialBalance, UmiFinancialBalanceArchiveSchema, UmiFinancialBalanceArchiveBound, UmiFinancialBalanceArchiveWrite, UmiFinancialBalanceArchiveRead, UmiFinancialBalanceArchiveValidate)
