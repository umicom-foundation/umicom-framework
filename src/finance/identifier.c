/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/identifier.c
 *
 * PURPOSE:
 *   Validate and compare stable financial identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test identifier behaviour without starting a complete product.
 */

#include <string.h>
#include "../base/value_archive_internal.h"
#include "umicom/finance/identifier.h"
/* Check that financial id satisfies its contract before another service relies on it. */
/*
 * Search only inside the actual fixed-size field. strlen would already read
 * beyond an unterminated identifier before its returned length could be tested.
 * Comparisons below validate both operands before using C string operations.
 */
int umi_financial_id_valid(const UmiFinancialId *id){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (id == NULL) return 0;
    if (memchr(id->value, '\0', sizeof(id->value)) == NULL) return 0;
return id!=NULL && id->value[0]!='\0' && memchr(id->value, '\0', sizeof(id->value)) != NULL;}
/*
 * Provide the financial id equal operation used by this module and its client
 * applications.
 */
int umi_financial_id_equal(const UmiFinancialId *left,const UmiFinancialId *right){return umi_financial_id_valid(left) && umi_financial_id_valid(right) && strcmp(left->value,right->value)==0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialIdArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa56cb8be3b274508);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialId *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialIdArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialId *)0)->value) - 1U;
}
static void UmiFinancialIdArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialId *value)
{
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
}
static void UmiFinancialIdArchiveRead(UmiArchiveReader *reader, UmiFinancialId *value)
{
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
}
static UmiStatus UmiFinancialIdArchiveValidate(const UmiFinancialId *value)
{
    return umi_financial_id_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_financial_id_archive_encode, umi_financial_id_archive_decode,
    UmiFinancialId, UmiFinancialIdArchiveSchema, UmiFinancialIdArchiveBound, UmiFinancialIdArchiveWrite, UmiFinancialIdArchiveRead, UmiFinancialIdArchiveValidate)
