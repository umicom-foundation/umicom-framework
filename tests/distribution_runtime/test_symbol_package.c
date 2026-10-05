/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_symbol_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for debug symbol package metadata and build-id matching.
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
#include "umicom/distribution/runtime/symbol_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/symbol_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrSymbolPackageTransferEqual(const UmiDrSymbolPackage *a, const UmiDrSymbolPackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->build_id, b->build_id) == 0 &&
        strcmp(a->digest, b->digest) == 0 &&
        a->size_bytes == b->size_bytes;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrSymbolPackageTransferTails(UmiDrSymbolPackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->build_id) + 1U;
        memset(value->build_id + used, 0xa5, sizeof(value->build_id) - used);
    }
    {
        size_t used = strlen(value->digest) + 1U;
        memset(value->digest + used, 0xa5, sizeof(value->digest) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrSymbolPackageTransferMalformed(const UmiDrSymbolPackage *sample)
{
    (void)sample;
    {
        UmiDrSymbolPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_symbol_package_valid(&invalid)) ||
            umi_dr_symbol_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrSymbolPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.build_id, 'x', sizeof(invalid.build_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_symbol_package_valid(&invalid)) ||
            umi_dr_symbol_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated build_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrSymbolPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.digest, 'x', sizeof(invalid.digest));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_symbol_package_valid(&invalid)) ||
            umi_dr_symbol_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated digest was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrSymbolPackageTransferCases, UmiDrSymbolPackage,
    umi_dr_symbol_package_archive_encode, umi_dr_symbol_package_archive_decode,
    UmiDrSymbolPackageTransferEqual, UmiDrSymbolPackageTransferTails, UmiDrSymbolPackageTransferMalformed)

int main(void) {
    UmiDrSymbolPackage value; umi_dr_symbol_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"sym")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.build_id,sizeof(value.build_id),"b")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); CHECK(umi_dr_symbol_package_valid(&value));
    if (UmiDrSymbolPackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_symbol_package_fingerprint(&value) != 0U);
    return 0;
}
