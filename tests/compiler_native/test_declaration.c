/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_declaration.c
 *
 * PURPOSE:
 *   Regression coverage for describe top-level and local c declarations independently from parser implementation details.
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
#include "umicom/compiler/native/declaration.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/compiler/native/declaration.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiNativeDeclarationTransferEqual(const UmiNativeDeclaration *a, const UmiNativeDeclaration *b)
{
    return a->node_id == b->node_id &&
        a->kind == b->kind &&
        strcmp(a->name, b->name) == 0 &&
        a->type_id == b->type_id &&
        a->external_linkage == b->external_linkage &&
        a->internal_linkage == b->internal_linkage &&
        a->definition == b->definition;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiNativeDeclarationTransferTails(UmiNativeDeclaration *value)
{
    (void)value;
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiNativeDeclarationTransferMalformed(const UmiNativeDeclaration *sample)
{
    (void)sample;
    {
        UmiNativeDeclaration invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_nc_declaration_validate(&invalid) != UMI_STATUS_OK) ||
            umi_nc_declaration_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiNativeDeclarationTransferCases, UmiNativeDeclaration,
    umi_nc_declaration_archive_encode, umi_nc_declaration_archive_decode,
    UmiNativeDeclarationTransferEqual, UmiNativeDeclarationTransferTails, UmiNativeDeclarationTransferMalformed)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiNativeDeclarationConditionalArchiveCases, UmiNativeDeclaration,
    umi_nc_declaration_archive_encode, umi_nc_declaration_archive_decode_if_unchanged,
    UmiNativeDeclarationTransferEqual, UmiNativeDeclarationTransferTails, UmiNativeDeclarationTransferMalformed)



/* A rejected replacement must preserve the previously accepted declaration value. */
static int UmiNativeDeclarationConstructionCases(void)
{
    UmiNativeDeclaration *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_declaration_init(value, 1U, UMI_NC_DECL_FUNCTION, "main", 2U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_declaration_init(value, 1U, UMI_NC_DECL_FUNCTION, oversized, 2U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_declaration_init(value, 1U, UMI_NC_DECL_FUNCTION, value->name, 2U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeDeclarationConstructionCases() != 0) return 1;
 UmiNativeDeclaration d;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_declaration_init(&d,1U,UMI_NC_DECL_FUNCTION,"main",2U)!=UMI_STATUS_OK)return 1;d.definition=true;d.external_linkage=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_declaration_validate(&d)!=UMI_STATUS_OK)return 2;
    if (UmiNativeDeclarationTransferCases(&d) != 0) return 1;
    if (UmiNativeDeclarationConditionalArchiveCases(&d) != 0) return 1;
return 0; }
