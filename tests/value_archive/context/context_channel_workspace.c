/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_workspace.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/workspace.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/workspace.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkspaceContextTransferEqual(const UmiWorkspaceContext *a, const UmiWorkspaceContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->profile_id, b->profile_id) == 0 &&
        strcmp(a->root_path, b->root_path) == 0 &&
        strcmp(a->perspective_id, b->perspective_id) == 0 &&
        strcmp(a->layout_id, b->layout_id) == 0 &&
        a->trusted == b->trusted &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkspaceContextTransferTails(UmiWorkspaceContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
    {
        size_t used = strlen(value->root_path) + 1U;
        memset(value->root_path + used, 0xa5, sizeof(value->root_path) - used);
    }
    {
        size_t used = strlen(value->perspective_id) + 1U;
        memset(value->perspective_id + used, 0xa5, sizeof(value->perspective_id) - used);
    }
    {
        size_t used = strlen(value->layout_id) + 1U;
        memset(value->layout_id + used, 0xa5, sizeof(value->layout_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkspaceContextTransferMalformed(const UmiWorkspaceContext *sample)
{
    (void)sample;
    {
        UmiWorkspaceContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workspace_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workspace_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkspaceContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workspace_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workspace_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkspaceContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root_path, 'x', sizeof(invalid.root_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workspace_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workspace_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkspaceContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.perspective_id, 'x', sizeof(invalid.perspective_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workspace_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workspace_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated perspective_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkspaceContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.layout_id, 'x', sizeof(invalid.layout_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workspace_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workspace_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated layout_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkspaceContextTransferCases, UmiWorkspaceContext,
    umi_workspace_context_archive_encode, umi_workspace_context_archive_decode,
    UmiWorkspaceContextTransferEqual, UmiWorkspaceContextTransferTails, UmiWorkspaceContextTransferMalformed)

int main(void)
{
    UmiWorkspaceContext value;
    umi_workspace_context_init(&value);
    value.workspace_id[0] = 's';
    value.profile_id[0] = 's';
    value.root_path[0] = 's';
    value.perspective_id[0] = 's';
    value.layout_id[0] = 's';
    value.trusted = true;
    value.revision = (uint64_t)17U;
    if (umi_workspace_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiWorkspaceContextTransferCases(&value) != 0) return 1;

    return 0;
}
