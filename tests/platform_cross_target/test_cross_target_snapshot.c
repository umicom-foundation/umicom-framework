/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_target_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross target snapshot cross-target capability.
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
#include "umicom/platform/cross_target/cross_target_snapshot.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/cross_target_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtCrossTargetSnapshotTransferEqual(const UmiCtCrossTargetSnapshot *a, const UmiCtCrossTargetSnapshot *b)
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
        strcmp(a->abi, b->abi) == 0 &&
        a->cpu_features == b->cpu_features &&
        a->cpu_count == b->cpu_count &&
        a->memory_bytes == b->memory_bytes &&
        a->page_size == b->page_size &&
        a->health.health == b->health.health &&
        a->health.blockers == b->health.blockers &&
        a->health.warnings == b->health.warnings &&
        a->health.readiness_percent == b->health.readiness_percent &&
        a->fingerprint == b->fingerprint &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtCrossTargetSnapshotTransferTails(UmiCtCrossTargetSnapshot *value)
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
    {
        size_t used = strlen(value->abi) + 1U;
        memset(value->abi + used, 0xa5, sizeof(value->abi) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtCrossTargetSnapshotTransferMalformed(const UmiCtCrossTargetSnapshot *sample)
{
    (void)sample;
    {
        UmiCtCrossTargetSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.triple, 'x', sizeof(invalid.target.triple));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_target_snapshot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_target_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.triple was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossTargetSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target.vendor, 'x', sizeof(invalid.target.vendor));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_target_snapshot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_target_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target.vendor was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCtCrossTargetSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.abi, 'x', sizeof(invalid.abi));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ct_cross_target_snapshot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ct_cross_target_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated abi was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtCrossTargetSnapshotTransferCases, UmiCtCrossTargetSnapshot,
    umi_ct_cross_target_snapshot_archive_encode, umi_ct_cross_target_snapshot_archive_decode,
    UmiCtCrossTargetSnapshotTransferEqual, UmiCtCrossTargetSnapshotTransferTails, UmiCtCrossTargetSnapshotTransferMalformed)

int main(void){UmiCtCrossTargetSnapshot s={0};s.target.architecture=UMI_CT_ARCH_RISCV64;CHECK(umi_ct_copy(s.abi,sizeof(s.abi),"lp64d")==UMI_STATUS_OK);s.cpu_count=4U;s.page_size=4096U;s.fingerprint=1U;s.health.health=UMI_CT_HEALTH_READY;CHECK(umi_ct_cross_target_snapshot_validate(&s)==UMI_STATUS_OK);
    if (UmiCtCrossTargetSnapshotTransferCases(&s) != 0) return 1;
s.health.health=UMI_CT_HEALTH_BLOCKED;CHECK(umi_ct_cross_target_snapshot_validate(&s)==UMI_STATUS_UNAVAILABLE);return 0;}
