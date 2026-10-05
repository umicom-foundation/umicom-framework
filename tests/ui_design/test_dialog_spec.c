/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_dialog_spec.c
 *
 * PURPOSE:
 *   Verify the semantic dialog spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
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
#include "umicom/ui/design/dialog_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/dialog_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignDialogSpecTransferEqual(const UmiDesignDialogSpec *a, const UmiDesignDialogSpec *b)
{
    return a->width_class == b->width_class &&
        a->action_count == b->action_count &&
        a->modal == b->modal &&
        a->destructive_action == b->destructive_action;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignDialogSpecTransferTails(UmiDesignDialogSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignDialogSpecTransferMalformed(const UmiDesignDialogSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignDialogSpecTransferCases, UmiDesignDialogSpec,
    umi_design_dialog_spec_archive_encode, umi_design_dialog_spec_archive_decode,
    UmiDesignDialogSpecTransferEqual, UmiDesignDialogSpecTransferTails, UmiDesignDialogSpecTransferMalformed)

int main(void){UmiDesignDialogSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_dialog_spec_init(&s,UMI_DESIGN_SIZE_MEDIUM,2U,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignDialogSpecTransferCases(&s) != 0) return 1;
return s.modal?0:2;}
