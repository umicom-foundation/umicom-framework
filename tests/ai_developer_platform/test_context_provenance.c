/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_provenance.c
 *
 * PURPOSE:
 *   Implement the test context provenance behavior for
 *   Umicom Framework.
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
#include "umicom/ai/developer_platform/context_provenance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ai/developer_platform/context_provenance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAiDevContextProvenanceTransferEqual(const UmiAiDevContextProvenance *a, const UmiAiDevContextProvenance *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->revision == b->revision &&
        a->flags == b->flags &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAiDevContextProvenanceTransferTails(UmiAiDevContextProvenance *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAiDevContextProvenanceTransferMalformed(const UmiAiDevContextProvenance *sample)
{
    (void)sample;
    {
        UmiAiDevContextProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_dev_context_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_dev_context_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAiDevContextProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ai_dev_context_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ai_dev_context_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAiDevContextProvenanceTransferCases, UmiAiDevContextProvenance,
    umi_ai_dev_context_provenance_archive_encode, umi_ai_dev_context_provenance_archive_decode,
    UmiAiDevContextProvenanceTransferEqual, UmiAiDevContextProvenanceTransferTails, UmiAiDevContextProvenanceTransferMalformed)

int main(void) {
    UmiAiDevContextProvenance v;
    umi_ai_dev_context_provenance_init(&v);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_dev_context_provenance_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ai_dev_context_provenance_validate(&v) != UMI_STATUS_OK) return 2;
    if (UmiAiDevContextProvenanceTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_dev_context_provenance_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
