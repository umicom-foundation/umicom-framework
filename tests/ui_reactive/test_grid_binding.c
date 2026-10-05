/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_grid_binding.c
 *
 * PURPOSE:
 *   Exercise the grid binding reactive UI contract.
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
#include "umicom/ui/reactive/grid_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/grid_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveGridBindingTransferEqual(const UmiUiReactiveGridBinding *a, const UmiUiReactiveGridBinding *b)
{
    return strcmp(a->grid_id, b->grid_id) == 0 &&
        strcmp(a->provider_path, b->provider_path) == 0 &&
        strcmp(a->query_path, b->query_path) == 0 &&
        strcmp(a->selection_path, b->selection_path) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveGridBindingTransferTails(UmiUiReactiveGridBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->grid_id) + 1U;
        memset(value->grid_id + used, 0xa5, sizeof(value->grid_id) - used);
    }
    {
        size_t used = strlen(value->provider_path) + 1U;
        memset(value->provider_path + used, 0xa5, sizeof(value->provider_path) - used);
    }
    {
        size_t used = strlen(value->query_path) + 1U;
        memset(value->query_path + used, 0xa5, sizeof(value->query_path) - used);
    }
    {
        size_t used = strlen(value->selection_path) + 1U;
        memset(value->selection_path + used, 0xa5, sizeof(value->selection_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveGridBindingTransferMalformed(const UmiUiReactiveGridBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveGridBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.grid_id, 'x', sizeof(invalid.grid_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_grid_binding_valid(&invalid)) ||
            umi_ui_reactive_grid_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated grid_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveGridBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_path, 'x', sizeof(invalid.provider_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_grid_binding_valid(&invalid)) ||
            umi_ui_reactive_grid_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveGridBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.query_path, 'x', sizeof(invalid.query_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_grid_binding_valid(&invalid)) ||
            umi_ui_reactive_grid_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated query_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveGridBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.selection_path, 'x', sizeof(invalid.selection_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_grid_binding_valid(&invalid)) ||
            umi_ui_reactive_grid_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated selection_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveGridBindingTransferCases, UmiUiReactiveGridBinding,
    umi_ui_reactive_grid_binding_archive_encode, umi_ui_reactive_grid_binding_archive_decode,
    UmiUiReactiveGridBindingTransferEqual, UmiUiReactiveGridBindingTransferTails, UmiUiReactiveGridBindingTransferMalformed)

int main(void) { UmiUiReactiveGridBinding item; umi_ui_reactive_grid_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveGridBinding populated = item;
    (void)snprintf(populated.grid_id, sizeof(populated.grid_id), "field-0");
    (void)snprintf(populated.provider_path, sizeof(populated.provider_path), "field-1");
    (void)snprintf(populated.query_path, sizeof(populated.query_path), "field-2");
    (void)snprintf(populated.selection_path, sizeof(populated.selection_path), "field-3");
    if (UmiUiReactiveGridBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_grid_binding_valid(&item) ? 0 : 1; }
