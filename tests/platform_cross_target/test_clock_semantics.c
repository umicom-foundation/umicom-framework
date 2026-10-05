/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_clock_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the clock semantics cross-target capability.
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
#include "umicom/platform/cross_target/clock_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/clock_semantics.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtClockSemanticsTransferEqual(const UmiCtClockSemantics *a, const UmiCtClockSemantics *b)
{
    return a->monotonic == b->monotonic &&
        a->wall_clock == b->wall_clock &&
        a->high_resolution == b->high_resolution &&
        a->frequency_hz == b->frequency_hz &&
        a->resolution_ns == b->resolution_ns;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtClockSemanticsTransferTails(UmiCtClockSemantics *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtClockSemanticsTransferMalformed(const UmiCtClockSemantics *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtClockSemanticsTransferCases, UmiCtClockSemantics,
    umi_ct_clock_semantics_archive_encode, umi_ct_clock_semantics_archive_decode,
    UmiCtClockSemanticsTransferEqual, UmiCtClockSemanticsTransferTails, UmiCtClockSemanticsTransferMalformed)

int main(void){UmiCtClockSemantics s={true,true,true,UINT64_C(1000000),1000U};CHECK(umi_ct_clock_semantics_validate(&s)==UMI_STATUS_OK);
    if (UmiCtClockSemanticsTransferCases(&s) != 0) return 1;
CHECK(umi_ct_clock_ticks_to_ns(&s,1000U)==1000000U);return 0;}
