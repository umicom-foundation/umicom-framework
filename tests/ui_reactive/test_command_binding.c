/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_binding.c
 *
 * PURPOSE:
 *   Exercise the command binding reactive UI contract.
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
#include "umicom/ui/reactive/command_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/command_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveCommandBindingTransferEqual(const UmiUiReactiveCommandBinding *a, const UmiUiReactiveCommandBinding *b)
{
    return strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->command_id, b->command_id) == 0 &&
        strcmp(a->parameter_path, b->parameter_path) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveCommandBindingTransferTails(UmiUiReactiveCommandBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->parameter_path) + 1U;
        memset(value->parameter_path + used, 0xa5, sizeof(value->parameter_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveCommandBindingTransferMalformed(const UmiUiReactiveCommandBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveCommandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_command_binding_valid(&invalid)) ||
            umi_ui_reactive_command_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveCommandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_command_binding_valid(&invalid)) ||
            umi_ui_reactive_command_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveCommandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parameter_path, 'x', sizeof(invalid.parameter_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_command_binding_valid(&invalid)) ||
            umi_ui_reactive_command_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parameter_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveCommandBindingTransferCases, UmiUiReactiveCommandBinding,
    umi_ui_reactive_command_binding_archive_encode, umi_ui_reactive_command_binding_archive_decode,
    UmiUiReactiveCommandBindingTransferEqual, UmiUiReactiveCommandBindingTransferTails, UmiUiReactiveCommandBindingTransferMalformed)

int main(void) { UmiUiReactiveCommandBinding item; umi_ui_reactive_command_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveCommandBinding populated = item;
    (void)snprintf(populated.binding_id, sizeof(populated.binding_id), "field-0");
    (void)snprintf(populated.command_id, sizeof(populated.command_id), "field-1");
    (void)snprintf(populated.parameter_path, sizeof(populated.parameter_path), "field-2");
    populated.enabled = true;
    if (UmiUiReactiveCommandBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_command_binding_valid(&item) ? 0 : 1; }
