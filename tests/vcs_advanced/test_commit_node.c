/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_node.c
 *
 * PURPOSE:
 *   Validate describe one commit in the framework-owned history graph.
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
#include "umicom/vcs/advanced/commit_node.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/commit_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedCommitNodeTransferEqual(const UmiVcsAdvancedCommitNode *a, const UmiVcsAdvancedCommitNode *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->oid, b->oid) == 0 &&
        strcmp(a->subject, b->subject) == 0 &&
        strcmp(a->author, b->author) == 0 &&
        a->timestamp_seconds == b->timestamp_seconds &&
        a->parent_count == b->parent_count &&
        a->generation == b->generation &&
        a->merge_commit == b->merge_commit;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedCommitNodeTransferTails(UmiVcsAdvancedCommitNode *value)
{
    (void)value;
    {
        size_t used = strlen(value->oid) + 1U;
        memset(value->oid + used, 0xa5, sizeof(value->oid) - used);
    }
    {
        size_t used = strlen(value->subject) + 1U;
        memset(value->subject + used, 0xa5, sizeof(value->subject) - used);
    }
    {
        size_t used = strlen(value->author) + 1U;
        memset(value->author + used, 0xa5, sizeof(value->author) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedCommitNodeTransferMalformed(const UmiVcsAdvancedCommitNode *sample)
{
    (void)sample;
    {
        UmiVcsAdvancedCommitNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.oid, 'x', sizeof(invalid.oid));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_commit_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_commit_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated oid was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedCommitNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject, 'x', sizeof(invalid.subject));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_commit_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_commit_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedCommitNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.author, 'x', sizeof(invalid.author));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_commit_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_commit_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated author was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedCommitNodeTransferCases, UmiVcsAdvancedCommitNode,
    umi_vcs_advanced_commit_node_archive_encode, umi_vcs_advanced_commit_node_archive_decode,
    UmiVcsAdvancedCommitNodeTransferEqual, UmiVcsAdvancedCommitNodeTransferTails, UmiVcsAdvancedCommitNodeTransferMalformed)

int main(void)
{
    UmiVcsAdvancedCommitNode value;
    umi_vcs_advanced_commit_node_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_node_validate(&value) == UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_node_set(&value, "abc123", "subject", "author") != UMI_STATUS_OK) return 2;
    value.parent_count = 2U;
    value.merge_commit = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_node_validate(&value) != UMI_STATUS_OK) return 3;
    if (UmiVcsAdvancedCommitNodeTransferCases(&value) != 0) return 1;

    return 0;
}
