/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/rate_index.c
 *
 * PURPOSE:
 *   Implement reusable rate-index metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/rate_index.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_rate_index_init(UmiRateIndex *item,const char *id,const char *name,const char *code,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->index_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_rate_index_is_valid(const UmiRateIndex *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->index_id.value, '\0', sizeof(item->index_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
    if (memchr(item->code, '\0', sizeof(item->code)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->index_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRateIndexArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5d04ff0e1debbde);
    schema = (schema ^ (uint64_t)sizeof(((UmiRateIndex *)0)->index_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRateIndex *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRateIndex *)0)->code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRateIndexArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRateIndex *)0)->index_id.value) - 1U +
        8U + sizeof(((UmiRateIndex *)0)->name) - 1U +
        8U + sizeof(((UmiRateIndex *)0)->code) - 1U +
        8U +
        8U;
}
static void UmiRateIndexArchiveWrite(UmiArchiveWriter *writer, const UmiRateIndex *value)
{
    UmiArchiveWriteText(writer, value->index_id.value, sizeof(value->index_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiRateIndexArchiveRead(UmiArchiveReader *reader, UmiRateIndex *value)
{
    UmiArchiveReadText(reader, value->index_id.value, sizeof(value->index_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    value->state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRateIndexArchiveValidate(const UmiRateIndex *value)
{
    return umi_rate_index_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rate_index_archive_encode, umi_rate_index_archive_decode,
    UmiRateIndex, UmiRateIndexArchiveSchema, UmiRateIndexArchiveBound, UmiRateIndexArchiveWrite, UmiRateIndexArchiveRead, UmiRateIndexArchiveValidate)
