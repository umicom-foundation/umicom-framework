/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_source_navigation.c
 *
 * PURPOSE:
 *   Implement the test editor source navigation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework source location and navigation history tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_history.h"

/*
 * Exercise location and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorSourceLocation location(const char *uri,
                                        uint64_t line,
                                        uint64_t column)
{
    UmiEditorSourceLocation value;
    assert(umi_editor_source_location_initialize(&value,
                                                  uri,
                                                  line,
                                                  column) == UMI_STATUS_OK);
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/source_location.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorSourceLocationTransferEqual(const UmiEditorSourceLocation *a, const UmiEditorSourceLocation *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->kind == b->kind &&
        a->line == b->line &&
        a->column == b->column &&
        a->end_line == b->end_line &&
        a->end_column == b->end_column &&
        a->byte_offset == b->byte_offset &&
        a->end_byte_offset == b->end_byte_offset &&
        a->document_revision == b->document_revision &&
        a->score == b->score &&
        strcmp(a->uri, b->uri) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->symbol_id, b->symbol_id) == 0 &&
        strcmp(a->preview, b->preview) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorSourceLocationTransferTails(UmiEditorSourceLocation *value)
{
    (void)value;
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->symbol_id) + 1U;
        memset(value->symbol_id + used, 0xa5, sizeof(value->symbol_id) - used);
    }
    {
        size_t used = strlen(value->preview) + 1U;
        memset(value->preview + used, 0xa5, sizeof(value->preview) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorSourceLocationTransferMalformed(const UmiEditorSourceLocation *sample)
{
    (void)sample;
    {
        UmiEditorSourceLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.uri, 'x', sizeof(invalid.uri));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_editor_source_location_validate(&invalid) != UMI_STATUS_OK) ||
            umi_editor_source_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated uri was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorSourceLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_editor_source_location_validate(&invalid) != UMI_STATUS_OK) ||
            umi_editor_source_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorSourceLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.symbol_id, 'x', sizeof(invalid.symbol_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_editor_source_location_validate(&invalid) != UMI_STATUS_OK) ||
            umi_editor_source_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated symbol_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorSourceLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.preview, 'x', sizeof(invalid.preview));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_editor_source_location_validate(&invalid) != UMI_STATUS_OK) ||
            umi_editor_source_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated preview was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorSourceLocationTransferCases, UmiEditorSourceLocation,
    umi_editor_source_location_archive_encode, umi_editor_source_location_archive_decode,
    UmiEditorSourceLocationTransferEqual, UmiEditorSourceLocationTransferTails, UmiEditorSourceLocationTransferMalformed)

int main(void)
{
    UmiEditorNavigationHistory *history = NULL;
    UmiEditorNavigationHistorySnapshot snapshot;
    UmiEditorSourceLocation first = location("file:///project/alpha.c", 4U, 2U);
    UmiEditorSourceLocation second = location("file:///project/beta.c", 8U, 1U);
    UmiEditorSourceLocation third = location("file:///project/gamma.c", 12U, 0U);
    UmiEditorSourceLocation branch = location("file:///project/branch.c", 3U, 7U);
    UmiEditorSourceLocation current;
    char formatted[1200];

    assert(umi_editor_source_location_validate(&first) == UMI_STATUS_OK);
    if (UmiEditorSourceLocationTransferCases(&first) != 0) return 1;

    assert(umi_editor_source_location_format(&first,
                                              formatted,
                                              sizeof(formatted)) ==
           UMI_STATUS_OK);
    assert(strcmp(formatted, "file:///project/alpha.c:5:3") == 0);
    assert(umi_editor_navigation_history_create(3U, &history) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &first) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &second) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &third) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_go_back(history, &current) == UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&current, &second));
    assert(umi_editor_navigation_history_record(history, &branch) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_go_forward(history, &current) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_editor_navigation_history_go_back(history, &current) == UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&current, &second));
    assert(umi_editor_navigation_history_snapshot(history, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.count == 3U);
    assert(snapshot.can_go_back);
    assert(snapshot.can_go_forward);

    /* Adjacent visits to one position update metadata without growing history. */
    assert(umi_editor_navigation_history_go_forward(history, &current) == UMI_STATUS_OK);
    (void)strcpy(branch.preview, "branch preview");
    assert(umi_editor_navigation_history_record(history, &branch) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_snapshot(history, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.count == 3U);
    assert(umi_editor_navigation_history_current(history, &current) == UMI_STATUS_OK);
    assert(strcmp(current.preview, "branch preview") == 0);

    umi_editor_navigation_history_destroy(history);
    return 0;
}
