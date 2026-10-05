/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_dependency.c
 *
 * PURPOSE:
 *   Focused regression coverage for native runtime-library dependency and availability policy.
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
#include "umicom/distribution/runtime/runtime_dependency.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/runtime_dependency.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrRuntimeDependencyTransferEqual(const UmiDrRuntimeDependency *a, const UmiDrRuntimeDependency *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->soname, b->soname) == 0 &&
        a->minimum_version.major == b->minimum_version.major &&
        a->minimum_version.minor == b->minimum_version.minor &&
        a->minimum_version.patch == b->minimum_version.patch &&
        a->system_provided == b->system_provided &&
        a->bundled == b->bundled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrRuntimeDependencyTransferTails(UmiDrRuntimeDependency *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->soname) + 1U;
        memset(value->soname + used, 0xa5, sizeof(value->soname) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrRuntimeDependencyTransferMalformed(const UmiDrRuntimeDependency *sample)
{
    (void)sample;
    {
        UmiDrRuntimeDependency invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_runtime_dependency_valid(&invalid)) ||
            umi_dr_runtime_dependency_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrRuntimeDependency invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.soname, 'x', sizeof(invalid.soname));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_runtime_dependency_valid(&invalid)) ||
            umi_dr_runtime_dependency_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated soname was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrRuntimeDependencyTransferCases, UmiDrRuntimeDependency,
    umi_dr_runtime_dependency_archive_encode, umi_dr_runtime_dependency_archive_decode,
    UmiDrRuntimeDependencyTransferEqual, UmiDrRuntimeDependencyTransferTails, UmiDrRuntimeDependencyTransferMalformed)

int main(void) {
    UmiDrRuntimeDependency value; umi_dr_runtime_dependency_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"libc")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.soname,sizeof(value.soname),"libc.so.6")==UMI_STATUS_OK); value.system_provided=true; CHECK(umi_dr_runtime_dependency_valid(&value));
    if (UmiDrRuntimeDependencyTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_runtime_dependency_fingerprint(&value) != 0U);
    return 0;
}
