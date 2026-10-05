/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/product_id.c
 *
 * PURPOSE:
 *   Implement typed product id assignment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/product_id.h"
#include "../../base/value_archive_internal.h"

/* Assign identifier. */ UmiStatus umi_product_id_set(UmiProductId *id,const char *value){if(id==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_financial_id_assign(&id->id,value);}
/* Validate identifier. */ bool umi_product_id_is_valid(const UmiProductId *id){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (id == NULL) return 0;
    if (memchr(id->id.value, '\0', sizeof(id->id.value)) == NULL) return 0;
return id!=NULL&&umi_financial_id_is_valid(&id->id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProductIdArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xafeaa1193bfb4fbc);
    schema = (schema ^ (uint64_t)sizeof(((UmiProductId *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProductIdArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProductId *)0)->id.value) - 1U;
}
static void UmiProductIdArchiveWrite(UmiArchiveWriter *writer, const UmiProductId *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
}
static void UmiProductIdArchiveRead(UmiArchiveReader *reader, UmiProductId *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
}
static UmiStatus UmiProductIdArchiveValidate(const UmiProductId *value)
{
    return umi_product_id_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_product_id_archive_encode, umi_product_id_archive_decode,
    UmiProductId, UmiProductIdArchiveSchema, UmiProductIdArchiveBound, UmiProductIdArchiveWrite, UmiProductIdArchiveRead, UmiProductIdArchiveValidate)
