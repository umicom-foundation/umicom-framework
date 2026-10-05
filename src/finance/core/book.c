/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/book.c
 *
 * PURPOSE:
 *   Implement legal-entity-owned financial books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/book.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_book_init(UmiFinancialBook *item,const char *id,const char *name,const char *parent_id){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->book_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_book_is_valid(const UmiFinancialBook *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->book_id.value, '\0', sizeof(item->book_id.value)) == NULL) return 0;
    if (memchr(item->parent_id.value, '\0', sizeof(item->parent_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->book_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialBookArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x80bd466fcf43b60c);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialBook *)0)->book_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialBook *)0)->parent_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialBook *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialBookArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialBook *)0)->book_id.value) - 1U +
        8U + sizeof(((UmiFinancialBook *)0)->parent_id.value) - 1U +
        8U + sizeof(((UmiFinancialBook *)0)->name) - 1U +
        8U;
}
static void UmiFinancialBookArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialBook *value)
{
    UmiArchiveWriteText(writer, value->book_id.value, sizeof(value->book_id.value));
    UmiArchiveWriteText(writer, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiFinancialBookArchiveRead(UmiArchiveReader *reader, UmiFinancialBook *value)
{
    UmiArchiveReadText(reader, value->book_id.value, sizeof(value->book_id.value));
    UmiArchiveReadText(reader, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFinancialBookArchiveValidate(const UmiFinancialBook *value)
{
    return umi_book_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_book_archive_encode, umi_book_archive_decode,
    UmiFinancialBook, UmiFinancialBookArchiveSchema, UmiFinancialBookArchiveBound, UmiFinancialBookArchiveWrite, UmiFinancialBookArchiveRead, UmiFinancialBookArchiveValidate)
