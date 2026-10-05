/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_tree_binding.c
 *
 * PURPOSE:
 *   Exercise the tree binding reactive UI contract.
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
#include "umicom/ui/reactive/tree_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/tree_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveTreeBindingTransferEqual(const UmiUiReactiveTreeBinding *a, const UmiUiReactiveTreeBinding *b)
{
    return strcmp(a->tree_id, b->tree_id) == 0 &&
        strcmp(a->provider_path, b->provider_path) == 0 &&
        strcmp(a->expansion_path, b->expansion_path) == 0 &&
        strcmp(a->selection_path, b->selection_path) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveTreeBindingTransferTails(UmiUiReactiveTreeBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->tree_id) + 1U;
        memset(value->tree_id + used, 0xa5, sizeof(value->tree_id) - used);
    }
    {
        size_t used = strlen(value->provider_path) + 1U;
        memset(value->provider_path + used, 0xa5, sizeof(value->provider_path) - used);
    }
    {
        size_t used = strlen(value->expansion_path) + 1U;
        memset(value->expansion_path + used, 0xa5, sizeof(value->expansion_path) - used);
    }
    {
        size_t used = strlen(value->selection_path) + 1U;
        memset(value->selection_path + used, 0xa5, sizeof(value->selection_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveTreeBindingTransferMalformed(const UmiUiReactiveTreeBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveTreeBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.tree_id, 'x', sizeof(invalid.tree_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_tree_binding_valid(&invalid)) ||
            umi_ui_reactive_tree_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated tree_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveTreeBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_path, 'x', sizeof(invalid.provider_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_tree_binding_valid(&invalid)) ||
            umi_ui_reactive_tree_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveTreeBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.expansion_path, 'x', sizeof(invalid.expansion_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_tree_binding_valid(&invalid)) ||
            umi_ui_reactive_tree_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated expansion_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveTreeBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.selection_path, 'x', sizeof(invalid.selection_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_tree_binding_valid(&invalid)) ||
            umi_ui_reactive_tree_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated selection_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveTreeBindingTransferCases, UmiUiReactiveTreeBinding,
    umi_ui_reactive_tree_binding_archive_encode, umi_ui_reactive_tree_binding_archive_decode,
    UmiUiReactiveTreeBindingTransferEqual, UmiUiReactiveTreeBindingTransferTails, UmiUiReactiveTreeBindingTransferMalformed)

int main(void) { UmiUiReactiveTreeBinding item; umi_ui_reactive_tree_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveTreeBinding populated = item;
    (void)snprintf(populated.tree_id, sizeof(populated.tree_id), "field-0");
    (void)snprintf(populated.provider_path, sizeof(populated.provider_path), "field-1");
    (void)snprintf(populated.expansion_path, sizeof(populated.expansion_path), "field-2");
    (void)snprintf(populated.selection_path, sizeof(populated.selection_path), "field-3");
    if (UmiUiReactiveTreeBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_tree_binding_valid(&item) ? 0 : 1; }
