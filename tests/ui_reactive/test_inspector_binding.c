/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_inspector_binding.c
 *
 * PURPOSE:
 *   Exercise the inspector binding reactive UI contract.
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
#include "umicom/ui/reactive/inspector_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/inspector_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveInspectorBindingTransferEqual(const UmiUiReactiveInspectorBinding *a, const UmiUiReactiveInspectorBinding *b)
{
    return strcmp(a->inspector_id, b->inspector_id) == 0 &&
        strcmp(a->subject_path, b->subject_path) == 0 &&
        strcmp(a->edit_path, b->edit_path) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveInspectorBindingTransferTails(UmiUiReactiveInspectorBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->inspector_id) + 1U;
        memset(value->inspector_id + used, 0xa5, sizeof(value->inspector_id) - used);
    }
    {
        size_t used = strlen(value->subject_path) + 1U;
        memset(value->subject_path + used, 0xa5, sizeof(value->subject_path) - used);
    }
    {
        size_t used = strlen(value->edit_path) + 1U;
        memset(value->edit_path + used, 0xa5, sizeof(value->edit_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveInspectorBindingTransferMalformed(const UmiUiReactiveInspectorBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveInspectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.inspector_id, 'x', sizeof(invalid.inspector_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_inspector_binding_valid(&invalid)) ||
            umi_ui_reactive_inspector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated inspector_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveInspectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_path, 'x', sizeof(invalid.subject_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_inspector_binding_valid(&invalid)) ||
            umi_ui_reactive_inspector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveInspectorBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.edit_path, 'x', sizeof(invalid.edit_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_inspector_binding_valid(&invalid)) ||
            umi_ui_reactive_inspector_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated edit_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveInspectorBindingTransferCases, UmiUiReactiveInspectorBinding,
    umi_ui_reactive_inspector_binding_archive_encode, umi_ui_reactive_inspector_binding_archive_decode,
    UmiUiReactiveInspectorBindingTransferEqual, UmiUiReactiveInspectorBindingTransferTails, UmiUiReactiveInspectorBindingTransferMalformed)

int main(void) { UmiUiReactiveInspectorBinding item; umi_ui_reactive_inspector_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveInspectorBinding populated = item;
    (void)snprintf(populated.inspector_id, sizeof(populated.inspector_id), "field-0");
    (void)snprintf(populated.subject_path, sizeof(populated.subject_path), "field-1");
    (void)snprintf(populated.edit_path, sizeof(populated.edit_path), "field-2");
    if (UmiUiReactiveInspectorBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_inspector_binding_valid(&item) ? 0 : 1; }
