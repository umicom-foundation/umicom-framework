/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_memory_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the memory semantics cross-target capability.
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
#include "umicom/platform/cross_target/memory_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/memory_semantics.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtMemorySemanticsTransferEqual(const UmiCtMemorySemantics *a, const UmiCtMemorySemantics *b)
{
    return a->page_size == b->page_size &&
        a->allocation_granularity == b->allocation_granularity &&
        a->virtual_memory == b->virtual_memory &&
        a->guard_pages == b->guard_pages &&
        a->executable_memory == b->executable_memory &&
        a->huge_pages == b->huge_pages;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtMemorySemanticsTransferTails(UmiCtMemorySemantics *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtMemorySemanticsTransferMalformed(const UmiCtMemorySemantics *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtMemorySemanticsTransferCases, UmiCtMemorySemantics,
    umi_ct_memory_semantics_archive_encode, umi_ct_memory_semantics_archive_decode,
    UmiCtMemorySemanticsTransferEqual, UmiCtMemorySemanticsTransferTails, UmiCtMemorySemanticsTransferMalformed)

int main(void){UmiCtMemorySemantics s={4096U,4096U,true,true,true,false};CHECK(umi_ct_memory_semantics_validate(&s)==UMI_STATUS_OK);
    if (UmiCtMemorySemanticsTransferCases(&s) != 0) return 1;
CHECK(umi_ct_memory_round_up(&s,4097U)==8192U);return 0;}
