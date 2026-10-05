/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_page_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the page policy cross-target capability.
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
#include "umicom/platform/cross_target/page_policy.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/page_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtPagePolicyTransferEqual(const UmiCtPagePolicy *a, const UmiCtPagePolicy *b)
{
    return a->base_page_size == b->base_page_size &&
        a->huge_page_size == b->huge_page_size &&
        a->huge_pages == b->huge_pages &&
        a->execute_never == b->execute_never;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtPagePolicyTransferTails(UmiCtPagePolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtPagePolicyTransferMalformed(const UmiCtPagePolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtPagePolicyTransferCases, UmiCtPagePolicy,
    umi_ct_page_policy_archive_encode, umi_ct_page_policy_archive_decode,
    UmiCtPagePolicyTransferEqual, UmiCtPagePolicyTransferTails, UmiCtPagePolicyTransferMalformed)

int main(void){UmiCtPagePolicy p={4096U,2097152U,true,true};CHECK(umi_ct_page_policy_validate(&p)==UMI_STATUS_OK);
    if (UmiCtPagePolicyTransferCases(&p) != 0) return 1;
CHECK(umi_ct_page_align_up(&p,4097U)==8192U);CHECK(umi_ct_page_count(&p,8193U)==3U);return 0;}
