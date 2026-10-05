/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/currency.c
 *
 * PURPOSE:
 *   Validate three-letter uppercase currency codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test currency behaviour without starting a complete product.
 */

#include "umicom/finance/currency.h"
#include "../base/value_archive_internal.h"
/* Provide the upper ascii operation used by this module and its client applications. */
static int upper_ascii(char c){return c>='A' && c<='Z';}
/* Check that currency satisfies its contract before another service relies on it. */
int umi_currency_valid(const UmiCurrency *currency){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (currency == NULL) return 0;
    if (memchr(currency->code, '\0', sizeof(currency->code)) == NULL) return 0;
return currency!=NULL && upper_ascii(currency->code[0]) && upper_ascii(currency->code[1]) && upper_ascii(currency->code[2]) && currency->code[3]=='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCurrencyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa671b1b5750cb492);
    schema = (schema ^ (uint64_t)sizeof(((UmiCurrency *)0)->code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCurrencyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCurrency *)0)->code) - 1U;
}
static void UmiCurrencyArchiveWrite(UmiArchiveWriter *writer, const UmiCurrency *value)
{
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
}
static void UmiCurrencyArchiveRead(UmiArchiveReader *reader, UmiCurrency *value)
{
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
}
static UmiStatus UmiCurrencyArchiveValidate(const UmiCurrency *value)
{
    return umi_currency_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_currency_archive_encode, umi_currency_archive_decode,
    UmiCurrency, UmiCurrencyArchiveSchema, UmiCurrencyArchiveBound, UmiCurrencyArchiveWrite, UmiCurrencyArchiveRead, UmiCurrencyArchiveValidate)
