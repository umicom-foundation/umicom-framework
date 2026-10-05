/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/portfolio.c
 *
 * PURPOSE:
 *   Implement portfolio groupings under books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/portfolio.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_portfolio_init(UmiFinancialPortfolio *item,const char *id,const char *name,const char *parent_id){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->portfolio_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_portfolio_is_valid(const UmiFinancialPortfolio *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->portfolio_id.value, '\0', sizeof(item->portfolio_id.value)) == NULL) return 0;
    if (memchr(item->parent_id.value, '\0', sizeof(item->parent_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->portfolio_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialPortfolioArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x092f24cd52ad859b);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialPortfolio *)0)->portfolio_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialPortfolio *)0)->parent_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialPortfolio *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialPortfolioArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialPortfolio *)0)->portfolio_id.value) - 1U +
        8U + sizeof(((UmiFinancialPortfolio *)0)->parent_id.value) - 1U +
        8U + sizeof(((UmiFinancialPortfolio *)0)->name) - 1U +
        8U;
}
static void UmiFinancialPortfolioArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialPortfolio *value)
{
    UmiArchiveWriteText(writer, value->portfolio_id.value, sizeof(value->portfolio_id.value));
    UmiArchiveWriteText(writer, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiFinancialPortfolioArchiveRead(UmiArchiveReader *reader, UmiFinancialPortfolio *value)
{
    UmiArchiveReadText(reader, value->portfolio_id.value, sizeof(value->portfolio_id.value));
    UmiArchiveReadText(reader, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFinancialPortfolioArchiveValidate(const UmiFinancialPortfolio *value)
{
    return umi_portfolio_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_portfolio_archive_encode, umi_portfolio_archive_decode,
    UmiFinancialPortfolio, UmiFinancialPortfolioArchiveSchema, UmiFinancialPortfolioArchiveBound, UmiFinancialPortfolioArchiveWrite, UmiFinancialPortfolioArchiveRead, UmiFinancialPortfolioArchiveValidate)
