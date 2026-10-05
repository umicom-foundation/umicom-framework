/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_direction.c
 *
 * PURPOSE:
 *   Exercise the binding direction reactive UI contract.
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
#include "umicom/ui/reactive/binding_direction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/binding_direction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBindingDirectionPolicyTransferEqual(const UmiUiReactiveBindingDirectionPolicy *a, const UmiUiReactiveBindingDirectionPolicy *b)
{
    return a->direction == b->direction &&
        a->trigger == b->trigger &&
        a->propagate_initial == b->propagate_initial;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBindingDirectionPolicyTransferTails(UmiUiReactiveBindingDirectionPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBindingDirectionPolicyTransferMalformed(const UmiUiReactiveBindingDirectionPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBindingDirectionPolicyTransferCases, UmiUiReactiveBindingDirectionPolicy,
    umi_ui_reactive_binding_direction_archive_encode, umi_ui_reactive_binding_direction_archive_decode,
    UmiUiReactiveBindingDirectionPolicyTransferEqual, UmiUiReactiveBindingDirectionPolicyTransferTails, UmiUiReactiveBindingDirectionPolicyTransferMalformed)

int main(void) { UmiUiReactiveBindingDirectionPolicy item; umi_ui_reactive_binding_direction_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBindingDirectionPolicy populated = item;
    populated.propagate_initial = true;
    if (UmiUiReactiveBindingDirectionPolicyTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_binding_direction_valid(&item) ? 0 : 1; }
