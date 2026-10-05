/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_change_event.c
 *
 * PURPOSE:
 *   Exercise the change event reactive UI contract.
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
#include "umicom/ui/reactive/change_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/change_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveChangeEventTransferEqual(const UmiUiReactiveChangeEvent *a, const UmiUiReactiveChangeEvent *b)
{
    return strcmp(a->path, b->path) == 0 &&
        a->before_value.kind == b->before_value.kind &&
        a->before_value.boolean_value == b->before_value.boolean_value &&
        a->before_value.integer_value == b->before_value.integer_value &&
        a->before_value.real_value == b->before_value.real_value &&
        strcmp(a->before_value.string_value, b->before_value.string_value) == 0 &&
        a->after_value.kind == b->after_value.kind &&
        a->after_value.boolean_value == b->after_value.boolean_value &&
        a->after_value.integer_value == b->after_value.integer_value &&
        a->after_value.real_value == b->after_value.real_value &&
        strcmp(a->after_value.string_value, b->after_value.string_value) == 0 &&
        a->sequence == b->sequence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveChangeEventTransferTails(UmiUiReactiveChangeEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->before_value.string_value) + 1U;
        memset(value->before_value.string_value + used, 0xa5, sizeof(value->before_value.string_value) - used);
    }
    {
        size_t used = strlen(value->after_value.string_value) + 1U;
        memset(value->after_value.string_value + used, 0xa5, sizeof(value->after_value.string_value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveChangeEventTransferMalformed(const UmiUiReactiveChangeEvent *sample)
{
    (void)sample;
    {
        UmiUiReactiveChangeEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_change_event_valid(&invalid)) ||
            umi_ui_reactive_change_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveChangeEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.before_value.string_value, 'x', sizeof(invalid.before_value.string_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_change_event_valid(&invalid)) ||
            umi_ui_reactive_change_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated before_value.string_value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveChangeEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.after_value.string_value, 'x', sizeof(invalid.after_value.string_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_change_event_valid(&invalid)) ||
            umi_ui_reactive_change_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated after_value.string_value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveChangeEventTransferCases, UmiUiReactiveChangeEvent,
    umi_ui_reactive_change_event_archive_encode, umi_ui_reactive_change_event_archive_decode,
    UmiUiReactiveChangeEventTransferEqual, UmiUiReactiveChangeEventTransferTails, UmiUiReactiveChangeEventTransferMalformed)

int main(void) { UmiUiReactiveChangeEvent item; umi_ui_reactive_change_event_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveChangeEvent populated = item;
    (void)snprintf(populated.path, sizeof(populated.path), "field-0");
    populated.before_value.boolean_value = (int)4;
    populated.before_value.integer_value = (int64_t)5;
    populated.before_value.real_value = 6.25;
    (void)snprintf(populated.before_value.string_value, sizeof(populated.before_value.string_value), "field-5");
    populated.after_value.boolean_value = (int)9;
    populated.after_value.integer_value = (int64_t)10;
    populated.after_value.real_value = 11.25;
    (void)snprintf(populated.after_value.string_value, sizeof(populated.after_value.string_value), "field-10");
    populated.sequence = (uint64_t)13;
    if (UmiUiReactiveChangeEventTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_change_event_valid(&item) ? 0 : 1; }
