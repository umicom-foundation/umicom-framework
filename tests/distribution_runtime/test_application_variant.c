/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_variant.c
 *
 * PURPOSE:
 *   Focused regression coverage for platform-specific application variants without moving reusable logic into products.
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
#include "umicom/distribution/runtime/application_variant.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/application_variant.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrApplicationVariantTransferEqual(const UmiDrApplicationVariant *a, const UmiDrApplicationVariant *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->platform == b->platform &&
        a->architecture == b->architecture &&
        a->preferred_format == b->preferred_format &&
        strcmp(a->entrypoint, b->entrypoint) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrApplicationVariantTransferTails(UmiDrApplicationVariant *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->entrypoint) + 1U;
        memset(value->entrypoint + used, 0xa5, sizeof(value->entrypoint) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrApplicationVariantTransferMalformed(const UmiDrApplicationVariant *sample)
{
    (void)sample;
    {
        UmiDrApplicationVariant invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_variant_valid(&invalid)) ||
            umi_dr_application_variant_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationVariant invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entrypoint, 'x', sizeof(invalid.entrypoint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_variant_valid(&invalid)) ||
            umi_dr_application_variant_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entrypoint was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrApplicationVariantTransferCases, UmiDrApplicationVariant,
    umi_dr_application_variant_archive_encode, umi_dr_application_variant_archive_decode,
    UmiDrApplicationVariantTransferEqual, UmiDrApplicationVariantTransferTails, UmiDrApplicationVariantTransferMalformed)

int main(void) {
    UmiDrApplicationVariant value; umi_dr_application_variant_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"win-x64") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.entrypoint,sizeof(value.entrypoint),"bin/app.exe") == UMI_STATUS_OK); CHECK(umi_dr_application_variant_valid(&value));
    if (UmiDrApplicationVariantTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_application_variant_fingerprint(&value) != 0U);
    return 0;
}
