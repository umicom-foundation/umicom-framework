/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/product_descriptor.c
 *
 * PURPOSE:
 *   Describe reusable financial products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/product_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_product_descriptor_init(UmiProductDescriptor *item,const char *id,const char *name,const char *code,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->product_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_product_descriptor_is_valid(const UmiProductDescriptor *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->product_id.value, '\0', sizeof(item->product_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
    if (memchr(item->code, '\0', sizeof(item->code)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->product_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProductDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x27615cdab5eb13ac);
    schema = (schema ^ (uint64_t)sizeof(((UmiProductDescriptor *)0)->product_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProductDescriptor *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProductDescriptor *)0)->code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProductDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProductDescriptor *)0)->product_id.value) - 1U +
        8U + sizeof(((UmiProductDescriptor *)0)->name) - 1U +
        8U + sizeof(((UmiProductDescriptor *)0)->code) - 1U +
        8U +
        8U;
}
static void UmiProductDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiProductDescriptor *value)
{
    UmiArchiveWriteText(writer, value->product_id.value, sizeof(value->product_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiProductDescriptorArchiveRead(UmiArchiveReader *reader, UmiProductDescriptor *value)
{
    UmiArchiveReadText(reader, value->product_id.value, sizeof(value->product_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    value->state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiProductDescriptorArchiveValidate(const UmiProductDescriptor *value)
{
    return umi_product_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_product_descriptor_archive_encode, umi_product_descriptor_archive_decode,
    UmiProductDescriptor, UmiProductDescriptorArchiveSchema, UmiProductDescriptorArchiveBound, UmiProductDescriptorArchiveWrite, UmiProductDescriptorArchiveRead, UmiProductDescriptorArchiveValidate)
