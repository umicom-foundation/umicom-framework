/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_platform_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for runtime operating-system descriptors and minimum platform revision requirements.
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
#include "umicom/distribution/runtime/platform_descriptor.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/platform_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrPlatformDescriptorTransferEqual(const UmiDrPlatformDescriptor *a, const UmiDrPlatformDescriptor *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->platform == b->platform &&
        a->minimum_version.major == b->minimum_version.major &&
        a->minimum_version.minor == b->minimum_version.minor &&
        a->minimum_version.patch == b->minimum_version.patch &&
        a->desktop == b->desktop &&
        a->server == b->server;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrPlatformDescriptorTransferTails(UmiDrPlatformDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrPlatformDescriptorTransferMalformed(const UmiDrPlatformDescriptor *sample)
{
    (void)sample;
    {
        UmiDrPlatformDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_platform_descriptor_valid(&invalid)) ||
            umi_dr_platform_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrPlatformDescriptorTransferCases, UmiDrPlatformDescriptor,
    umi_dr_platform_descriptor_archive_encode, umi_dr_platform_descriptor_archive_decode,
    UmiDrPlatformDescriptorTransferEqual, UmiDrPlatformDescriptorTransferTails, UmiDrPlatformDescriptorTransferMalformed)

int main(void) {
    UmiDrPlatformDescriptor value; umi_dr_platform_descriptor_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"linux") == UMI_STATUS_OK); value.platform=UMI_DR_PLATFORM_LINUX; CHECK(umi_dr_platform_descriptor_valid(&value));
    if (UmiDrPlatformDescriptorTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_platform_descriptor_fingerprint(&value) != 0U);
    return 0;
}
