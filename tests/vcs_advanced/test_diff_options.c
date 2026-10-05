/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_options.c
 *
 * PURPOSE:
 *   Validate define deterministic user comparison options shared by every frontend.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
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
#include "umicom/vcs/advanced/diff_options.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/diff_options.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedDiffOptionsTransferEqual(const UmiVcsAdvancedDiffOptions *a, const UmiVcsAdvancedDiffOptions *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->whitespace == b->whitespace &&
        a->context_lines == b->context_lines &&
        a->ignore_case == b->ignore_case &&
        a->detect_moves == b->detect_moves &&
        a->semantic == b->semantic &&
        a->word_diff == b->word_diff &&
        a->treat_crlf_as_lf == b->treat_crlf_as_lf;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedDiffOptionsTransferTails(UmiVcsAdvancedDiffOptions *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedDiffOptionsTransferMalformed(const UmiVcsAdvancedDiffOptions *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedDiffOptionsTransferCases, UmiVcsAdvancedDiffOptions,
    umi_vcs_advanced_diff_options_archive_encode, umi_vcs_advanced_diff_options_archive_decode,
    UmiVcsAdvancedDiffOptionsTransferEqual, UmiVcsAdvancedDiffOptionsTransferTails, UmiVcsAdvancedDiffOptionsTransferMalformed)

int main(void)
{
    UmiVcsAdvancedDiffOptions value;
    umi_vcs_advanced_diff_options_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_options_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedDiffOptionsTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_diff_options_fingerprint(&value) == 0U) return 3;
    return 0;
}
