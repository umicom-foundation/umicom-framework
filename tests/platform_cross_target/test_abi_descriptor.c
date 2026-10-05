/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_abi_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the abi descriptor cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
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
#include "umicom/platform/cross_target/abi_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/abi_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtAbiDescriptorTransferEqual(const UmiCtAbiDescriptor *a, const UmiCtAbiDescriptor *b)
{
    return strcmp(a->abi_id, b->abi_id) == 0 &&
        a->data_model == b->data_model &&
        a->calling_convention == b->calling_convention &&
        a->pointer_bits == b->pointer_bits &&
        a->stack_alignment == b->stack_alignment &&
        a->long_double_bits == b->long_double_bits &&
        a->hard_float == b->hard_float;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtAbiDescriptorTransferTails(UmiCtAbiDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->abi_id) + 1U;
        memset(value->abi_id + used, 0xa5, sizeof(value->abi_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtAbiDescriptorTransferMalformed(const UmiCtAbiDescriptor *sample)
{
    (void)sample;
    {
        UmiCtAbiDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.abi_id, 'x', sizeof(invalid.abi_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_abi_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_abi_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated abi_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtAbiDescriptorTransferCases, UmiCtAbiDescriptor,
    umi_ct_abi_descriptor_archive_encode, umi_ct_abi_descriptor_archive_decode,
    UmiCtAbiDescriptorTransferEqual, UmiCtAbiDescriptorTransferTails, UmiCtAbiDescriptorTransferMalformed)

int main(void){UmiCtAbiDescriptor d={"lp64d",UMI_CT_DATA_LP64,UMI_CT_CALL_RISCV,64U,16U,128U,true};CHECK(umi_ct_abi_descriptor_validate(&d)==UMI_STATUS_OK);
    if (UmiCtAbiDescriptorTransferCases(&d) != 0) return 1;
d.pointer_bits=32U;CHECK(umi_ct_abi_descriptor_validate(&d)==UMI_STATUS_INVALID_STATE);return 0;}
