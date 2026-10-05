/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_repository_state.c
 *
 * PURPOSE:
 *   Validate aggregate branch/upstream and in-progress git operation state.
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
#include "umicom/vcs/advanced/repository_state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/repository_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedRepositoryStateTransferEqual(const UmiVcsAdvancedRepositoryState *a, const UmiVcsAdvancedRepositoryState *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->branch, b->branch) == 0 &&
        strcmp(a->upstream, b->upstream) == 0 &&
        strcmp(a->head_oid, b->head_oid) == 0 &&
        a->ahead == b->ahead &&
        a->behind == b->behind &&
        a->conflicts == b->conflicts &&
        a->detached_head == b->detached_head &&
        a->merge_in_progress == b->merge_in_progress &&
        a->rebase_in_progress == b->rebase_in_progress &&
        a->cherry_pick_in_progress == b->cherry_pick_in_progress &&
        a->revert_in_progress == b->revert_in_progress &&
        a->bisect_in_progress == b->bisect_in_progress;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedRepositoryStateTransferTails(UmiVcsAdvancedRepositoryState *value)
{
    (void)value;
    {
        size_t used = strlen(value->branch) + 1U;
        memset(value->branch + used, 0xa5, sizeof(value->branch) - used);
    }
    {
        size_t used = strlen(value->upstream) + 1U;
        memset(value->upstream + used, 0xa5, sizeof(value->upstream) - used);
    }
    {
        size_t used = strlen(value->head_oid) + 1U;
        memset(value->head_oid + used, 0xa5, sizeof(value->head_oid) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedRepositoryStateTransferMalformed(const UmiVcsAdvancedRepositoryState *sample)
{
    (void)sample;
    {
        UmiVcsAdvancedRepositoryState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.branch, 'x', sizeof(invalid.branch));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_repository_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_repository_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated branch was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedRepositoryState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.upstream, 'x', sizeof(invalid.upstream));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_repository_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_repository_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated upstream was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedRepositoryState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.head_oid, 'x', sizeof(invalid.head_oid));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_repository_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_repository_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated head_oid was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedRepositoryStateTransferCases, UmiVcsAdvancedRepositoryState,
    umi_vcs_advanced_repository_state_archive_encode, umi_vcs_advanced_repository_state_archive_decode,
    UmiVcsAdvancedRepositoryStateTransferEqual, UmiVcsAdvancedRepositoryStateTransferTails, UmiVcsAdvancedRepositoryStateTransferMalformed)

int main(void)
{
    UmiVcsAdvancedRepositoryState value;
    umi_vcs_advanced_repository_state_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_repository_state_validate(&value) == UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_copy_text(value.head_oid, sizeof(value.head_oid), "abc") != UMI_STATUS_OK) return 2;
    value.ahead = 1U; value.behind = 2U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_repository_state_validate(&value) != UMI_STATUS_OK) return 3;
    if (UmiVcsAdvancedRepositoryStateTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_repository_state_diverged(&value)) return 4;
    return 0;
}
