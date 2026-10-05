/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_event_binding.c
 *
 * PURPOSE:
 *   Exercise the event binding reactive UI contract.
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
#include "umicom/ui/reactive/event_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/event_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveEventBindingTransferEqual(const UmiUiReactiveEventBinding *a, const UmiUiReactiveEventBinding *b)
{
    return strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->event_name, b->event_name) == 0 &&
        strcmp(a->action_id, b->action_id) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveEventBindingTransferTails(UmiUiReactiveEventBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->event_name) + 1U;
        memset(value->event_name + used, 0xa5, sizeof(value->event_name) - used);
    }
    {
        size_t used = strlen(value->action_id) + 1U;
        memset(value->action_id + used, 0xa5, sizeof(value->action_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveEventBindingTransferMalformed(const UmiUiReactiveEventBinding *sample)
{
    (void)sample;
    {
        UmiUiReactiveEventBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_event_binding_valid(&invalid)) ||
            umi_ui_reactive_event_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveEventBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_name, 'x', sizeof(invalid.event_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_event_binding_valid(&invalid)) ||
            umi_ui_reactive_event_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveEventBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_event_binding_valid(&invalid)) ||
            umi_ui_reactive_event_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveEventBindingTransferCases, UmiUiReactiveEventBinding,
    umi_ui_reactive_event_binding_archive_encode, umi_ui_reactive_event_binding_archive_decode,
    UmiUiReactiveEventBindingTransferEqual, UmiUiReactiveEventBindingTransferTails, UmiUiReactiveEventBindingTransferMalformed)

int main(void) { UmiUiReactiveEventBinding item; umi_ui_reactive_event_binding_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveEventBinding populated = item;
    (void)snprintf(populated.source_id, sizeof(populated.source_id), "field-0");
    (void)snprintf(populated.event_name, sizeof(populated.event_name), "field-1");
    (void)snprintf(populated.action_id, sizeof(populated.action_id), "field-2");
    populated.enabled = true;
    if (UmiUiReactiveEventBindingTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_event_binding_valid(&item) ? 0 : 1; }
