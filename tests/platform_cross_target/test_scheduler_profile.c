/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_scheduler_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the scheduler profile cross-target capability.
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
#include "umicom/platform/cross_target/scheduler_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/scheduler_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtSchedulerProfileTransferEqual(const UmiCtSchedulerProfile *a, const UmiCtSchedulerProfile *b)
{
    return a->scheduler_class == b->scheduler_class &&
        a->cpu_count == b->cpu_count &&
        a->timeslice_us == b->timeslice_us &&
        a->priority_levels == b->priority_levels &&
        a->affinity == b->affinity &&
        a->load_balancing == b->load_balancing;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtSchedulerProfileTransferTails(UmiCtSchedulerProfile *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtSchedulerProfileTransferMalformed(const UmiCtSchedulerProfile *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtSchedulerProfileTransferCases, UmiCtSchedulerProfile,
    umi_ct_scheduler_profile_archive_encode, umi_ct_scheduler_profile_archive_decode,
    UmiCtSchedulerProfileTransferEqual, UmiCtSchedulerProfileTransferTails, UmiCtSchedulerProfileTransferMalformed)

int main(void){UmiCtSchedulerProfile p={UMI_CT_SCHED_PREEMPTIVE,4U,2000U,32U,true,true};CHECK(umi_ct_scheduler_profile_validate(&p)==UMI_STATUS_OK);
    if (UmiCtSchedulerProfileTransferCases(&p) != 0) return 1;
p.timeslice_us=0U;CHECK(umi_ct_scheduler_profile_validate(&p)==UMI_STATUS_INVALID_STATE);return 0;}
