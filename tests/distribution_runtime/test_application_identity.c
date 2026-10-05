/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_identity.c
 *
 * PURPOSE:
 *   Focused regression coverage for stable application identity, publisher and product-family metadata.
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
#include "umicom/distribution/runtime/application_identity.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/application_identity.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrApplicationIdentityTransferEqual(const UmiDrApplicationIdentity *a, const UmiDrApplicationIdentity *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->publisher, b->publisher) == 0 &&
        strcmp(a->family, b->family) == 0 &&
        strcmp(a->product, b->product) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrApplicationIdentityTransferTails(UmiDrApplicationIdentity *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->publisher) + 1U;
        memset(value->publisher + used, 0xa5, sizeof(value->publisher) - used);
    }
    {
        size_t used = strlen(value->family) + 1U;
        memset(value->family + used, 0xa5, sizeof(value->family) - used);
    }
    {
        size_t used = strlen(value->product) + 1U;
        memset(value->product + used, 0xa5, sizeof(value->product) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrApplicationIdentityTransferMalformed(const UmiDrApplicationIdentity *sample)
{
    (void)sample;
    {
        UmiDrApplicationIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_identity_valid(&invalid)) ||
            umi_dr_application_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.publisher, 'x', sizeof(invalid.publisher));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_identity_valid(&invalid)) ||
            umi_dr_application_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated publisher was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.family, 'x', sizeof(invalid.family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_identity_valid(&invalid)) ||
            umi_dr_application_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated family was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.product, 'x', sizeof(invalid.product));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_identity_valid(&invalid)) ||
            umi_dr_application_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated product was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrApplicationIdentityTransferCases, UmiDrApplicationIdentity,
    umi_dr_application_identity_archive_encode, umi_dr_application_identity_archive_decode,
    UmiDrApplicationIdentityTransferEqual, UmiDrApplicationIdentityTransferTails, UmiDrApplicationIdentityTransferMalformed)

int main(void) {
    UmiDrApplicationIdentity value; umi_dr_application_identity_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"trader") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.publisher,sizeof(value.publisher),"Umicom Foundation") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.product,sizeof(value.product),"Umicom Trader") == UMI_STATUS_OK); CHECK(umi_dr_application_identity_valid(&value));
    if (UmiDrApplicationIdentityTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_application_identity_fingerprint(&value) != 0U);
    return 0;
}
