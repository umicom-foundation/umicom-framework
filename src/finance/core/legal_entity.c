/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/legal_entity.c
 *
 * PURPOSE:
 *   Implement legal entities shared by all financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/legal_entity.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_legal_entity_init(UmiLegalEntity *item,const char *id,const char *name,const char *code){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->entity_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_legal_entity_is_valid(const UmiLegalEntity *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->entity_id.value, '\0', sizeof(item->entity_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
    if (memchr(item->code, '\0', sizeof(item->code)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->entity_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiLegalEntityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x707bc30931390c8b);
    schema = (schema ^ (uint64_t)sizeof(((UmiLegalEntity *)0)->entity_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLegalEntity *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLegalEntity *)0)->code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiLegalEntityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiLegalEntity *)0)->entity_id.value) - 1U +
        8U + sizeof(((UmiLegalEntity *)0)->name) - 1U +
        8U + sizeof(((UmiLegalEntity *)0)->code) - 1U +
        8U;
}
static void UmiLegalEntityArchiveWrite(UmiArchiveWriter *writer, const UmiLegalEntity *value)
{
    UmiArchiveWriteText(writer, value->entity_id.value, sizeof(value->entity_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiLegalEntityArchiveRead(UmiArchiveReader *reader, UmiLegalEntity *value)
{
    UmiArchiveReadText(reader, value->entity_id.value, sizeof(value->entity_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiLegalEntityArchiveValidate(const UmiLegalEntity *value)
{
    return umi_legal_entity_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_legal_entity_archive_encode, umi_legal_entity_archive_decode,
    UmiLegalEntity, UmiLegalEntityArchiveSchema, UmiLegalEntityArchiveBound, UmiLegalEntityArchiveWrite, UmiLegalEntityArchiveRead, UmiLegalEntityArchiveValidate)
