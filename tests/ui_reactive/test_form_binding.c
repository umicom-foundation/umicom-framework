/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_form_binding.c
 *
 * PURPOSE:
 *   Exercise the form binding reactive UI contract.
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
#include "umicom/ui/reactive/form_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/form_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveFormBindingTransferEqual(const UmiUiReactiveFormBinding *a, const UmiUiReactiveFormBinding *b)
{
    return strcmp(a->form_id, b->form_id) == 0 &&
        strcmp(a->model_prefix, b->model_prefix) == 0 &&
        a->trigger == b->trigger &&
        a->validate_before_commit == b->validate_before_commit;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveFormBindingTransferTails(UmiUiReactiveFormBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->form_id) + 1U;
        memset(value->form_id + used, 0xa5, sizeof(value->form_id) - used);
    }
    {
        size_t used = strlen(value->model_prefix) + 1U;
        memset(value->model_prefix + used, 0xa5, sizeof(value->model_prefix) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveFormBindingTransferMalformed(const UmiUiReactiveFormBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveFormBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.form_id, 'x', sizeof(invalid.form_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_form_binding_valid(&invalid)) ||
            umi_ui_reactive_form_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated form_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveFormBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.model_prefix, 'x', sizeof(invalid.model_prefix));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_form_binding_valid(&invalid)) ||
            umi_ui_reactive_form_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated model_prefix was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveFormBindingTransferCases, UmiUiReactiveFormBinding,
    umi_ui_reactive_form_binding_archive_encode, umi_ui_reactive_form_binding_archive_decode,
    UmiUiReactiveFormBindingTransferEqual, UmiUiReactiveFormBindingTransferTails, UmiUiReactiveFormBindingTransferMalformed)

int main(void) { UmiUiReactiveFormBinding item; umi_ui_reactive_form_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveFormBinding populated = item;
    (void)snprintf(populated.form_id, sizeof(populated.form_id), "field-0");
    (void)snprintf(populated.model_prefix, sizeof(populated.model_prefix), "field-1");
    populated.validate_before_commit = true;
    if (UmiUiReactiveFormBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_form_binding_valid(&item) ? 0 : 1; }
