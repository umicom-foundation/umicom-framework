/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for named runtime profiles combining platform, architecture and capabilities.
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
#include "umicom/distribution/runtime/runtime_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/runtime_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrRuntimeProfileTransferEqual(const UmiDrRuntimeProfile *a, const UmiDrRuntimeProfile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->platform == b->platform &&
        a->architecture == b->architecture &&
        a->minimum_version.major == b->minimum_version.major &&
        a->minimum_version.minor == b->minimum_version.minor &&
        a->minimum_version.patch == b->minimum_version.patch &&
        a->required_capabilities == b->required_capabilities &&
        a->score_bias == b->score_bias;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrRuntimeProfileTransferTails(UmiDrRuntimeProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrRuntimeProfileTransferMalformed(const UmiDrRuntimeProfile *sample)
{
    (void)sample;
    {
        UmiDrRuntimeProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_runtime_profile_valid(&invalid)) ||
            umi_dr_runtime_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrRuntimeProfileTransferCases, UmiDrRuntimeProfile,
    umi_dr_runtime_profile_archive_encode, umi_dr_runtime_profile_archive_decode,
    UmiDrRuntimeProfileTransferEqual, UmiDrRuntimeProfileTransferTails, UmiDrRuntimeProfileTransferMalformed)

int main(void) {
    UmiDrRuntimeProfile value; umi_dr_runtime_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"linux-x64") == UMI_STATUS_OK); CHECK(umi_dr_runtime_profile_valid(&value));
    if (UmiDrRuntimeProfileTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_runtime_profile_fingerprint(&value) != 0U);
    return 0;
}
