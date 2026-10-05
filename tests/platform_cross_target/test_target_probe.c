/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_probe.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target probe cross-target capability.
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
#include "umicom/platform/cross_target/target_probe.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/target_probe.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtTargetProbeTransferEqual(const UmiCtTargetProbe *a, const UmiCtTargetProbe *b)
{
    return a->target.structure_size == b->target.structure_size &&
        a->target.api_version == b->target.api_version &&
        strcmp(a->target.triple, b->target.triple) == 0 &&
        strcmp(a->target.vendor, b->target.vendor) == 0 &&
        a->target.architecture == b->target.architecture &&
        a->target.operating_system == b->target.operating_system &&
        a->target.environment == b->target.environment &&
        a->target.pointer_bits == b->target.pointer_bits &&
        a->target.endian == b->target.endian &&
        a->cpu_count == b->cpu_count &&
        a->memory_bytes == b->memory_bytes &&
        a->page_size == b->page_size &&
        a->cpu_features == b->cpu_features &&
        a->confidence == b->confidence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtTargetProbeTransferTails(UmiCtTargetProbe *value)
{
    (void)value;
    {
        size_t used = strlen(value->target.triple) + 1U;
        memset(value->target.triple + used, 0xa5, sizeof(value->target.triple) - used);
    }
    {
        size_t used = strlen(value->target.vendor) + 1U;
        memset(value->target.vendor + used, 0xa5, sizeof(value->target.vendor) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtTargetProbeTransferMalformed(const UmiCtTargetProbe *sample)
{
    (void)sample;
    {
        UmiCtTargetProbe invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.triple, 'x', sizeof(invalid.target.triple));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_target_probe_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_target_probe_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.triple was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtTargetProbe invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.vendor, 'x', sizeof(invalid.target.vendor));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_target_probe_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_target_probe_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.vendor was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtTargetProbeTransferCases, UmiCtTargetProbe,
    umi_ct_target_probe_archive_encode, umi_ct_target_probe_archive_decode,
    UmiCtTargetProbeTransferEqual, UmiCtTargetProbeTransferTails, UmiCtTargetProbeTransferMalformed)

int main(void){UmiCtTargetProbe p={0};UmiCtTarget e={0};p.target.architecture=e.architecture=UMI_CT_ARCH_RISCV64;p.target.operating_system=e.operating_system=UMI_CT_OS_UMICOM;p.target.environment=e.environment=UMI_CT_ENV_UMICOM;p.target.pointer_bits=e.pointer_bits=64U;p.cpu_count=4U;p.page_size=4096U;p.confidence=100U;CHECK(umi_ct_target_probe_validate(&p)==UMI_STATUS_OK);
    if (UmiCtTargetProbeTransferCases(&p) != 0) return 1;
CHECK(umi_ct_target_probe_score(&p,&e)==100U);return 0;}
