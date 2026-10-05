/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_architecture_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for CPU architecture, ABI, pointer-width and endianness descriptors.
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
#include "umicom/distribution/runtime/architecture_descriptor.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/architecture_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrArchitectureDescriptorTransferEqual(const UmiDrArchitectureDescriptor *a, const UmiDrArchitectureDescriptor *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->architecture == b->architecture &&
        a->endian == b->endian &&
        a->pointer_bits == b->pointer_bits &&
        strcmp(a->abi, b->abi) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrArchitectureDescriptorTransferTails(UmiDrArchitectureDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->abi) + 1U;
        memset(value->abi + used, 0xa5, sizeof(value->abi) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrArchitectureDescriptorTransferMalformed(const UmiDrArchitectureDescriptor *sample)
{
    (void)sample;
    {
        UmiDrArchitectureDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_architecture_descriptor_valid(&invalid)) ||
            umi_dr_architecture_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrArchitectureDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.abi, 'x', sizeof(invalid.abi));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_architecture_descriptor_valid(&invalid)) ||
            umi_dr_architecture_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated abi was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrArchitectureDescriptorTransferCases, UmiDrArchitectureDescriptor,
    umi_dr_architecture_descriptor_archive_encode, umi_dr_architecture_descriptor_archive_decode,
    UmiDrArchitectureDescriptorTransferEqual, UmiDrArchitectureDescriptorTransferTails, UmiDrArchitectureDescriptorTransferMalformed)

int main(void) {
    UmiDrArchitectureDescriptor value; umi_dr_architecture_descriptor_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"x86_64") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.abi,sizeof(value.abi),"gnu") == UMI_STATUS_OK); CHECK(umi_dr_architecture_descriptor_valid(&value));
    if (UmiDrArchitectureDescriptorTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_architecture_descriptor_fingerprint(&value) != 0U);
    return 0;
}
