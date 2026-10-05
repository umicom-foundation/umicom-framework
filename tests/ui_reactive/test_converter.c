/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_converter.c
 *
 * PURPOSE:
 *   Exercise the converter reactive UI contract.
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
#include "umicom/ui/reactive/converter.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/converter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveConverterTransferEqual(const UmiUiReactiveConverter *a, const UmiUiReactiveConverter *b)
{
    return strcmp(a->converter_id, b->converter_id) == 0 &&
        a->source_kind == b->source_kind &&
        a->target_kind == b->target_kind &&
        a->supports_reverse == b->supports_reverse;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveConverterTransferTails(UmiUiReactiveConverter *value)
{
    (void)value;
    {
        size_t used = strlen(value->converter_id) + 1U;
        memset(value->converter_id + used, 0xa5, sizeof(value->converter_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveConverterTransferMalformed(const UmiUiReactiveConverter *sample)
{
    (void)sample;
    {
        UmiUiReactiveConverter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.converter_id, 'x', sizeof(invalid.converter_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_converter_valid(&invalid)) ||
            umi_ui_reactive_converter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated converter_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveConverterTransferCases, UmiUiReactiveConverter,
    umi_ui_reactive_converter_archive_encode, umi_ui_reactive_converter_archive_decode,
    UmiUiReactiveConverterTransferEqual, UmiUiReactiveConverterTransferTails, UmiUiReactiveConverterTransferMalformed)

int main(void) { UmiUiReactiveConverter item; umi_ui_reactive_converter_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveConverter populated = item;
    (void)snprintf(populated.converter_id, sizeof(populated.converter_id), "field-0");
    populated.supports_reverse = true;
    if (UmiUiReactiveConverterTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_converter_valid(&item) ? 0 : 1; }
