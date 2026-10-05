/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_component_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for optional Framework component package and dependency metadata.
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
#include "umicom/distribution/runtime/component_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/component_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrComponentPackageTransferEqual(const UmiDrComponentPackage *a, const UmiDrComponentPackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->component_id, b->component_id) == 0 &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->dependency_count == b->dependency_count &&
        a->optional == b->optional;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrComponentPackageTransferTails(UmiDrComponentPackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrComponentPackageTransferMalformed(const UmiDrComponentPackage *sample)
{
    (void)sample;
    {
        UmiDrComponentPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_component_package_valid(&invalid)) ||
            umi_dr_component_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrComponentPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_component_package_valid(&invalid)) ||
            umi_dr_component_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrComponentPackageTransferCases, UmiDrComponentPackage,
    umi_dr_component_package_archive_encode, umi_dr_component_package_archive_decode,
    UmiDrComponentPackageTransferEqual, UmiDrComponentPackageTransferTails, UmiDrComponentPackageTransferMalformed)

int main(void) {
    UmiDrComponentPackage value; umi_dr_component_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"component")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.component_id,sizeof(value.component_id),"ai")==UMI_STATUS_OK); CHECK(umi_dr_component_package_valid(&value));
    if (UmiDrComponentPackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_component_package_fingerprint(&value) != 0U);
    return 0;
}
