/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_toolbar_spec.c
 *
 * PURPOSE:
 *   Verify the semantic toolbar spec contract.
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
#include "umicom/ui/design/toolbar_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/toolbar_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignToolbarSpecTransferEqual(const UmiDesignToolbarSpec *a, const UmiDesignToolbarSpec *b)
{
    return a->orientation == b->orientation &&
        a->density == b->density &&
        a->preferred_items == b->preferred_items &&
        a->overflow_menu == b->overflow_menu;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignToolbarSpecTransferTails(UmiDesignToolbarSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignToolbarSpecTransferMalformed(const UmiDesignToolbarSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignToolbarSpecTransferCases, UmiDesignToolbarSpec,
    umi_design_toolbar_spec_archive_encode, umi_design_toolbar_spec_archive_decode,
    UmiDesignToolbarSpecTransferEqual, UmiDesignToolbarSpecTransferTails, UmiDesignToolbarSpecTransferMalformed)

int main(void){UmiDesignToolbarSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_toolbar_spec_init(&s,UMI_UI_HORIZONTAL,UMI_DESIGN_DENSITY_COMPACT,8U,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignToolbarSpecTransferCases(&s) != 0) return 1;
return s.overflow_menu?0:2;}
