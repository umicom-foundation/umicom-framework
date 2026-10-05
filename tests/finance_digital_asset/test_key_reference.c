/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_key_reference.c
 *
 * PURPOSE:
 *   Implement the test key reference behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/key_reference.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/key_reference.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalKeyReferenceTransferEqual(const UmiDigitalKeyReference *a, const UmiDigitalKeyReference *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->provider_reference, b->provider_reference) == 0 &&
        a->hardware_backed == b->hardware_backed &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalKeyReferenceTransferTails(UmiDigitalKeyReference *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->provider_reference) + 1U;
        memset(value->provider_reference + used, 0xa5, sizeof(value->provider_reference) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalKeyReferenceTransferMalformed(const UmiDigitalKeyReference *sample)
{
    (void)sample;
    {
        UmiDigitalKeyReference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_key_reference_valid(&invalid)) ||
            umi_digital_asset_key_reference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalKeyReference invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_reference, 'x', sizeof(invalid.provider_reference));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_key_reference_valid(&invalid)) ||
            umi_digital_asset_key_reference_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_reference was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalKeyReferenceTransferCases, UmiDigitalKeyReference,
    umi_digital_asset_key_reference_archive_encode, umi_digital_asset_key_reference_archive_decode,
    UmiDigitalKeyReferenceTransferEqual, UmiDigitalKeyReferenceTransferTails, UmiDigitalKeyReferenceTransferMalformed)

int main(void)
{
    UmiDigitalKeyReference value;
    CHECK(umi_digital_asset_key_reference_init(&value, "KEY-1", "hsm://slot/1", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_key_reference_valid(&value));
    if (UmiDigitalKeyReferenceTransferCases(&value) != 0) return 1;

    CHECK(value.hardware_backed);
    return 0;
}
