/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_kernel_boundary.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the kernel boundary cross-target capability.
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
#include "umicom/platform/cross_target/kernel_boundary.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/kernel_boundary.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtKernelBoundaryTransferEqual(const UmiCtKernelBoundary *a, const UmiCtKernelBoundary *b)
{
    return strcmp(a->boundary_id, b->boundary_id) == 0 &&
        a->caller == b->caller &&
        a->callee == b->callee &&
        a->copy_in == b->copy_in &&
        a->copy_out == b->copy_out &&
        a->privileged == b->privileged;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtKernelBoundaryTransferTails(UmiCtKernelBoundary *value)
{
    (void)value;
    {
        size_t used = strlen(value->boundary_id) + 1U;
        memset(value->boundary_id + used, 0xa5, sizeof(value->boundary_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtKernelBoundaryTransferMalformed(const UmiCtKernelBoundary *sample)
{
    (void)sample;
    {
        UmiCtKernelBoundary invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.boundary_id, 'x', sizeof(invalid.boundary_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_kernel_boundary_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_kernel_boundary_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated boundary_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtKernelBoundaryTransferCases, UmiCtKernelBoundary,
    umi_ct_kernel_boundary_archive_encode, umi_ct_kernel_boundary_archive_decode,
    UmiCtKernelBoundaryTransferEqual, UmiCtKernelBoundaryTransferTails, UmiCtKernelBoundaryTransferMalformed)

int main(void){UmiCtKernelBoundary b={"syscall",UMI_CT_DOMAIN_USER,UMI_CT_DOMAIN_KERNEL,true,true,true};CHECK(umi_ct_kernel_boundary_validate(&b)==UMI_STATUS_OK);
    if (UmiCtKernelBoundaryTransferCases(&b) != 0) return 1;
b.privileged=false;CHECK(umi_ct_kernel_boundary_validate(&b)==UMI_STATUS_INVALID_STATE);return 0;}
