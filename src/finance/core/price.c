/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/price.c
 *
 * PURPOSE:
 *   Implement financial price validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/price.h"
#include "../../base/value_archive_internal.h"

#include <math.h>
/* Initialize price. */ UmiStatus umi_price_init(UmiFinancialPrice *p,double value,uint8_t scale){if(p==NULL||!isfinite(value)||value<0.0||scale>12U)return UMI_STATUS_INVALID_ARGUMENT;p->value=value;p->scale=scale;return UMI_STATUS_OK;}
/* Validate price. */ bool umi_price_is_valid(const UmiFinancialPrice *p){return p!=NULL&&isfinite(p->value)&&p->value>=0.0&&p->scale<=12U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialPriceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa849c0d4f7458436);

    return schema;
}
static size_t UmiFinancialPriceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiFinancialPriceArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialPrice *value)
{
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scale);
}
static void UmiFinancialPriceArchiveRead(UmiArchiveReader *reader, UmiFinancialPrice *value)
{
    value->value = UmiArchiveReadDouble(reader);
    value->scale = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
}
static UmiStatus UmiFinancialPriceArchiveValidate(const UmiFinancialPrice *value)
{
    return umi_price_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_price_archive_encode, umi_price_archive_decode,
    UmiFinancialPrice, UmiFinancialPriceArchiveSchema, UmiFinancialPriceArchiveBound, UmiFinancialPriceArchiveWrite, UmiFinancialPriceArchiveRead, UmiFinancialPriceArchiveValidate)
