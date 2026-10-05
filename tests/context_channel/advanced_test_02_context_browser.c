/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_02_context_browser.c
 *
 * PURPOSE:
 *   Validate context browser sequence accounting, bounded fields and failure evidence.
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
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_browser.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_browser.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextBrowserTransferEqual(const UmiContextBrowser *a, const UmiContextBrowser *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->filter_text, b->filter_text) == 0 &&
        strcmp(a->category, b->category) == 0 &&
        strcmp(a->owner_id, b->owner_id) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        a->first_sequence == b->first_sequence &&
        a->last_sequence == b->last_sequence &&
        a->item_count == b->item_count &&
        a->failure_count == b->failure_count &&
        a->status == b->status &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextBrowserTransferTails(UmiContextBrowser *value)
{
    (void)value;
    {
        size_t used = strlen(value->filter_text) + 1U;
        memset(value->filter_text + used, 0xa5, sizeof(value->filter_text) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->owner_id) + 1U;
        memset(value->owner_id + used, 0xa5, sizeof(value->owner_id) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextBrowserTransferMalformed(const UmiContextBrowser *sample)
{
    (void)sample;
    {
        UmiContextBrowser invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.filter_text, 'x', sizeof(invalid.filter_text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_browser_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_browser_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated filter_text was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBrowser invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.category, 'x', sizeof(invalid.category));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_browser_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_browser_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated category was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBrowser invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.owner_id, 'x', sizeof(invalid.owner_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_browser_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_browser_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated owner_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBrowser invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_browser_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_browser_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextBrowserTransferCases, UmiContextBrowser,
    umi_context_browser_archive_encode, umi_context_browser_archive_decode,
    UmiContextBrowserTransferEqual, UmiContextBrowserTransferTails, UmiContextBrowserTransferMalformed)

int main(void)
{
    UmiContextBrowser state;
    umi_context_browser_init(&state);
    assert(umi_context_browser_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_browser_field(&state,0U),"alpha") == 0);
    assert(umi_context_browser_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_browser_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_browser_covers_sequence(&state,10U));
    assert(umi_context_browser_covers_sequence(&state,11U));
    assert(umi_context_browser_validate(&state) == UMI_STATUS_OK);
    if (UmiContextBrowserTransferCases(&state) != 0) return 1;

    return 0;
}
