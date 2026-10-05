/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_launch_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for named launch profile with environment, frontend and safe-mode controls.
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
#include "umicom/distribution/runtime/launch_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/launch_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrLaunchProfileTransferEqual(const UmiDrLaunchProfile *a, const UmiDrLaunchProfile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->launcher_id, b->launcher_id) == 0 &&
        strcmp(a->environment_id, b->environment_id) == 0 &&
        strcmp(a->frontend, b->frontend) == 0 &&
        a->safe_mode == b->safe_mode &&
        a->offline == b->offline;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrLaunchProfileTransferTails(UmiDrLaunchProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->launcher_id) + 1U;
        memset(value->launcher_id + used, 0xa5, sizeof(value->launcher_id) - used);
    }
    {
        size_t used = strlen(value->environment_id) + 1U;
        memset(value->environment_id + used, 0xa5, sizeof(value->environment_id) - used);
    }
    {
        size_t used = strlen(value->frontend) + 1U;
        memset(value->frontend + used, 0xa5, sizeof(value->frontend) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrLaunchProfileTransferMalformed(const UmiDrLaunchProfile *sample)
{
    (void)sample;
    {
        UmiDrLaunchProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launch_profile_valid(&invalid)) ||
            umi_dr_launch_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLaunchProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.launcher_id, 'x', sizeof(invalid.launcher_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launch_profile_valid(&invalid)) ||
            umi_dr_launch_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated launcher_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLaunchProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.environment_id, 'x', sizeof(invalid.environment_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launch_profile_valid(&invalid)) ||
            umi_dr_launch_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated environment_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLaunchProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.frontend, 'x', sizeof(invalid.frontend));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launch_profile_valid(&invalid)) ||
            umi_dr_launch_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated frontend was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrLaunchProfileTransferCases, UmiDrLaunchProfile,
    umi_dr_launch_profile_archive_encode, umi_dr_launch_profile_archive_decode,
    UmiDrLaunchProfileTransferEqual, UmiDrLaunchProfileTransferTails, UmiDrLaunchProfileTransferMalformed)

int main(void) {
    UmiDrLaunchProfile value; umi_dr_launch_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"normal")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.launcher_id,sizeof(value.launcher_id),"default")==UMI_STATUS_OK); CHECK(umi_dr_launch_profile_valid(&value));
    if (UmiDrLaunchProfileTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_launch_profile_fingerprint(&value) != 0U);
    return 0;
}
