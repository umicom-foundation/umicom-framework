/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_built_in_converters.c
 *
 * PURPOSE:
 *   Exercise the built in converters reactive UI contract.
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
#include "umicom/ui/reactive/built_in_converters.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/built_in_converters.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBuiltInConvertersTransferEqual(const UmiUiReactiveBuiltInConverters *a, const UmiUiReactiveBuiltInConverters *b)
{
    return a->allow_lossy_numeric == b->allow_lossy_numeric &&
        a->trim_strings == b->trim_strings;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBuiltInConvertersTransferTails(UmiUiReactiveBuiltInConverters *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBuiltInConvertersTransferMalformed(const UmiUiReactiveBuiltInConverters *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBuiltInConvertersTransferCases, UmiUiReactiveBuiltInConverters,
    umi_ui_reactive_built_in_converters_archive_encode, umi_ui_reactive_built_in_converters_archive_decode,
    UmiUiReactiveBuiltInConvertersTransferEqual, UmiUiReactiveBuiltInConvertersTransferTails, UmiUiReactiveBuiltInConvertersTransferMalformed)

int main(void) { UmiUiReactiveBuiltInConverters item; umi_ui_reactive_built_in_converters_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBuiltInConverters populated = item;
    populated.allow_lossy_numeric = true;
    populated.trim_strings = true;
    if (UmiUiReactiveBuiltInConvertersTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_built_in_converters_valid(&item) ? 0 : 1; }
