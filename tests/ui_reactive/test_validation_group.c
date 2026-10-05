/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_group.c
 *
 * PURPOSE:
 *   Exercise the validation group reactive UI contract.
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
#include "umicom/ui/reactive/validation_group.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/validation_group.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveValidationGroupTransferEqual(const UmiUiReactiveValidationGroup *a, const UmiUiReactiveValidationGroup *b)
{
    return a->total == b->total &&
        a->invalid == b->invalid &&
        a->warnings == b->warnings &&
        a->blocking == b->blocking;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveValidationGroupTransferTails(UmiUiReactiveValidationGroup *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveValidationGroupTransferMalformed(const UmiUiReactiveValidationGroup *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveValidationGroupTransferCases, UmiUiReactiveValidationGroup,
    umi_ui_reactive_validation_group_archive_encode, umi_ui_reactive_validation_group_archive_decode,
    UmiUiReactiveValidationGroupTransferEqual, UmiUiReactiveValidationGroupTransferTails, UmiUiReactiveValidationGroupTransferMalformed)

int main(void) { UmiUiReactiveValidationGroup item; umi_ui_reactive_validation_group_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveValidationGroup populated = item;
    populated.total = (size_t)2;
    populated.invalid = (size_t)3;
    populated.warnings = (size_t)4;
    populated.blocking = true;
    if (UmiUiReactiveValidationGroupTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_validation_group_valid(&item) ? 0 : 1; }
