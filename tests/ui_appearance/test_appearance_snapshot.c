/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_snapshot.c
 *
 * PURPOSE:
 *   Verify persist resolved appearance identity and revisions for deterministic session restore and visual tests.
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
#include "umicom/ui/appearance/appearance_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/appearance_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceAppearanceSnapshotTransferEqual(const UmiAppearanceAppearanceSnapshot *a, const UmiAppearanceAppearanceSnapshot *b)
{
    return strcmp(a->snapshot_id, b->snapshot_id) == 0 &&
        strcmp(a->profile_id, b->profile_id) == 0 &&
        strcmp(a->theme_pack_id, b->theme_pack_id) == 0 &&
        a->effective_scale == b->effective_scale &&
        a->semantic_revision == b->semantic_revision &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceAppearanceSnapshotTransferTails(UmiAppearanceAppearanceSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->snapshot_id) + 1U;
        memset(value->snapshot_id + used, 0xa5, sizeof(value->snapshot_id) - used);
    }
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
    {
        size_t used = strlen(value->theme_pack_id) + 1U;
        memset(value->theme_pack_id + used, 0xa5, sizeof(value->theme_pack_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceAppearanceSnapshotTransferMalformed(const UmiAppearanceAppearanceSnapshot *sample)
{
    (void)sample;
    {
        UmiAppearanceAppearanceSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.snapshot_id, 'x', sizeof(invalid.snapshot_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_snapshot_is_valid(&invalid)) ||
            umi_appearance_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated snapshot_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceAppearanceSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_snapshot_is_valid(&invalid)) ||
            umi_appearance_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceAppearanceSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.theme_pack_id, 'x', sizeof(invalid.theme_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_snapshot_is_valid(&invalid)) ||
            umi_appearance_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated theme_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceAppearanceSnapshotTransferCases, UmiAppearanceAppearanceSnapshot,
    umi_appearance_snapshot_archive_encode, umi_appearance_snapshot_archive_decode,
    UmiAppearanceAppearanceSnapshotTransferEqual, UmiAppearanceAppearanceSnapshotTransferTails, UmiAppearanceAppearanceSnapshotTransferMalformed)

int main(void) {
    UmiAppearanceAppearanceSnapshot item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_snapshot_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_snapshot_is_valid(&item)) return 2;
    if (UmiAppearanceAppearanceSnapshotTransferCases(&item) != 0) return 1;

    return 0;
}
