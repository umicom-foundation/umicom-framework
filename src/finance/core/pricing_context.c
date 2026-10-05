/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/pricing_context.c
 *
 * PURPOSE:
 *   Implement pricing context identity and valuation date.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/pricing_context.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_pricing_context_init(UmiPricingContext *item,const char *id,const char *name,const char *code,UmiFinancialDate effective_date){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->context_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(effective_date))return UMI_STATUS_INVALID_ARGUMENT; item->effective_date=effective_date; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_pricing_context_is_valid(const UmiPricingContext *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->context_id.value, '\0', sizeof(item->context_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
    if (memchr(item->code, '\0', sizeof(item->code)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->context_id)&&item->name[0]!='\0'&&item->code[0]!='\0'&&umi_financial_date_is_valid(item->effective_date);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPricingContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x42dc1938f482e999);
    schema = (schema ^ (uint64_t)sizeof(((UmiPricingContext *)0)->context_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPricingContext *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPricingContext *)0)->code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPricingContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPricingContext *)0)->context_id.value) - 1U +
        8U + sizeof(((UmiPricingContext *)0)->name) - 1U +
        8U + sizeof(((UmiPricingContext *)0)->code) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPricingContextArchiveWrite(UmiArchiveWriter *writer, const UmiPricingContext *value)
{
    UmiArchiveWriteText(writer, value->context_id.value, sizeof(value->context_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteSigned(writer, (int64_t)value->effective_date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->effective_date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->effective_date.day);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiPricingContextArchiveRead(UmiArchiveReader *reader, UmiPricingContext *value)
{
    UmiArchiveReadText(reader, value->context_id.value, sizeof(value->context_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    value->effective_date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->effective_date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->effective_date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPricingContextArchiveValidate(const UmiPricingContext *value)
{
    return umi_pricing_context_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_pricing_context_archive_encode, umi_pricing_context_archive_decode,
    UmiPricingContext, UmiPricingContextArchiveSchema, UmiPricingContextArchiveBound, UmiPricingContextArchiveWrite, UmiPricingContextArchiveRead, UmiPricingContextArchiveValidate)
