/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_evidence.c
 *
 * PURPOSE:
 *   Verify persist auditable foreground/background token and ratio evidence for conformance reports.
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
#include "umicom/ui/appearance/contrast_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/contrast_evidence.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceContrastEvidenceTransferEqual(const UmiAppearanceContrastEvidence *a, const UmiAppearanceContrastEvidence *b)
{
    return strcmp(a->evidence_id, b->evidence_id) == 0 &&
        strcmp(a->foreground_token, b->foreground_token) == 0 &&
        strcmp(a->background_token, b->background_token) == 0 &&
        a->ratio == b->ratio &&
        a->passed == b->passed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceContrastEvidenceTransferTails(UmiAppearanceContrastEvidence *value)
{
    (void)value;
    {
        size_t used = strlen(value->evidence_id) + 1U;
        memset(value->evidence_id + used, 0xa5, sizeof(value->evidence_id) - used);
    }
    {
        size_t used = strlen(value->foreground_token) + 1U;
        memset(value->foreground_token + used, 0xa5, sizeof(value->foreground_token) - used);
    }
    {
        size_t used = strlen(value->background_token) + 1U;
        memset(value->background_token + used, 0xa5, sizeof(value->background_token) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceContrastEvidenceTransferMalformed(const UmiAppearanceContrastEvidence *sample)
{
    (void)sample;
    {
        UmiAppearanceContrastEvidence invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.evidence_id, 'x', sizeof(invalid.evidence_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_contrast_evidence_is_valid(&invalid)) ||
            umi_appearance_contrast_evidence_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated evidence_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceContrastEvidence invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.foreground_token, 'x', sizeof(invalid.foreground_token));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_contrast_evidence_is_valid(&invalid)) ||
            umi_appearance_contrast_evidence_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated foreground_token was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceContrastEvidence invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.background_token, 'x', sizeof(invalid.background_token));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_contrast_evidence_is_valid(&invalid)) ||
            umi_appearance_contrast_evidence_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated background_token was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceContrastEvidenceTransferCases, UmiAppearanceContrastEvidence,
    umi_appearance_contrast_evidence_archive_encode, umi_appearance_contrast_evidence_archive_decode,
    UmiAppearanceContrastEvidenceTransferEqual, UmiAppearanceContrastEvidenceTransferTails, UmiAppearanceContrastEvidenceTransferMalformed)

int main(void) {
    UmiAppearanceContrastEvidence item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_contrast_evidence_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_contrast_evidence_is_valid(&item)) return 2;
    if (UmiAppearanceContrastEvidenceTransferCases(&item) != 0) return 1;

    return 0;
}
