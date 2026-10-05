/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cpu_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cpu profile cross-target capability.
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
#include "umicom/platform/cross_target/cpu_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/cpu_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtCpuProfileTransferEqual(const UmiCtCpuProfile *a, const UmiCtCpuProfile *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        a->architecture == b->architecture &&
        a->xlen == b->xlen &&
        a->required.bits == b->required.bits &&
        a->minimum_cores == b->minimum_cores;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtCpuProfileTransferTails(UmiCtCpuProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtCpuProfileTransferMalformed(const UmiCtCpuProfile *sample)
{
    (void)sample;
    {
        UmiCtCpuProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cpu_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cpu_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtCpuProfileTransferCases, UmiCtCpuProfile,
    umi_ct_cpu_profile_archive_encode, umi_ct_cpu_profile_archive_decode,
    UmiCtCpuProfileTransferEqual, UmiCtCpuProfileTransferTails, UmiCtCpuProfileTransferMalformed)

int main(void){UmiCtCpuProfile p={"rv64-base",UMI_CT_ARCH_RISCV64,64U,{1U},2U};UmiCtCpuFeatureSet a={1U};CHECK(umi_ct_cpu_profile_validate(&p)==UMI_STATUS_OK);
    if (UmiCtCpuProfileTransferCases(&p) != 0) return 1;
CHECK(umi_ct_cpu_profile_matches(&p,UMI_CT_ARCH_RISCV64,64U,4U,&a));CHECK(!umi_ct_cpu_profile_matches(&p,UMI_CT_ARCH_RISCV64,64U,1U,&a));return 0;}
