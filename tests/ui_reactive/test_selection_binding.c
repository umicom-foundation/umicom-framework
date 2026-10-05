/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_selection_binding.c
 *
 * PURPOSE:
 *   Exercise the selection binding reactive UI contract.
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
#include "umicom/ui/reactive/selection_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/selection_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveSelectionBindingTransferEqual(const UmiUiReactiveSelectionBinding *a, const UmiUiReactiveSelectionBinding *b)
{
    return strcmp(a->surface_id, b->surface_id) == 0 &&
        strcmp(a->selection_path, b->selection_path) == 0 &&
        a->two_way == b->two_way;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveSelectionBindingTransferTails(UmiUiReactiveSelectionBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->surface_id) + 1U;
        memset(value->surface_id + used, 0xa5, sizeof(value->surface_id) - used);
    }
    {
        size_t used = strlen(value->selection_path) + 1U;
        memset(value->selection_path + used, 0xa5, sizeof(value->selection_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveSelectionBindingTransferMalformed(const UmiUiReactiveSelectionBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveSelectionBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.surface_id, 'x', sizeof(invalid.surface_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_selection_binding_valid(&invalid)) ||
            umi_ui_reactive_selection_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated surface_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveSelectionBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.selection_path, 'x', sizeof(invalid.selection_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_selection_binding_valid(&invalid)) ||
            umi_ui_reactive_selection_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated selection_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveSelectionBindingTransferCases, UmiUiReactiveSelectionBinding,
    umi_ui_reactive_selection_binding_archive_encode, umi_ui_reactive_selection_binding_archive_decode,
    UmiUiReactiveSelectionBindingTransferEqual, UmiUiReactiveSelectionBindingTransferTails, UmiUiReactiveSelectionBindingTransferMalformed)

int main(void) { UmiUiReactiveSelectionBinding item; umi_ui_reactive_selection_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveSelectionBinding populated = item;
    (void)snprintf(populated.surface_id, sizeof(populated.surface_id), "field-0");
    (void)snprintf(populated.selection_path, sizeof(populated.selection_path), "field-1");
    populated.two_way = true;
    if (UmiUiReactiveSelectionBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_selection_binding_valid(&item) ? 0 : 1; }
