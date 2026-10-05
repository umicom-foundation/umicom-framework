/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_symbol.c
 *
 * PURPOSE:
 *   Regression coverage for describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/compiler/native/symbol.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/compiler/native/symbol.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiNativeSymbolTransferEqual(const UmiNativeSymbol *a, const UmiNativeSymbol *b)
{
    return a->id == b->id &&
        a->kind == b->kind &&
        strcmp(a->name, b->name) == 0 &&
        a->type_id == b->type_id &&
        a->scope_id == b->scope_id &&
        a->defined == b->defined &&
        a->external_linkage == b->external_linkage &&
        a->internal_linkage == b->internal_linkage;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiNativeSymbolTransferTails(UmiNativeSymbol *value)
{
    (void)value;
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiNativeSymbolTransferMalformed(const UmiNativeSymbol *sample)
{
    (void)sample;
    {
        UmiNativeSymbol invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_nc_symbol_validate(&invalid) != UMI_STATUS_OK) ||
            umi_nc_symbol_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiNativeSymbolTransferCases, UmiNativeSymbol,
    umi_nc_symbol_archive_encode, umi_nc_symbol_archive_decode,
    UmiNativeSymbolTransferEqual, UmiNativeSymbolTransferTails, UmiNativeSymbolTransferMalformed)

int main(void){UmiNativeSymbol s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_symbol_init(&s,1U,UMI_NC_SYMBOL_VARIABLE,"x",2U,1U)!=UMI_STATUS_OK)return 1;s.defined=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_symbol_validate(&s)!=UMI_STATUS_OK)return 2;
    if (UmiNativeSymbolTransferCases(&s) != 0) return 1;
return 0;}
