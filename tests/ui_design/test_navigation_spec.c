/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_navigation_spec.c
 *
 * PURPOSE:
 *   Verify the semantic navigation spec contract.
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
#include "umicom/ui/design/navigation_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/navigation_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignNavigationSpecTransferEqual(const UmiDesignNavigationSpec *a, const UmiDesignNavigationSpec *b)
{
    return a->placement == b->placement &&
        a->item_count == b->item_count &&
        a->collapsible == b->collapsible &&
        a->show_labels == b->show_labels;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignNavigationSpecTransferTails(UmiDesignNavigationSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignNavigationSpecTransferMalformed(const UmiDesignNavigationSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignNavigationSpecTransferCases, UmiDesignNavigationSpec,
    umi_design_navigation_spec_archive_encode, umi_design_navigation_spec_archive_decode,
    UmiDesignNavigationSpecTransferEqual, UmiDesignNavigationSpecTransferTails, UmiDesignNavigationSpecTransferMalformed)

int main(void){UmiDesignNavigationSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_navigation_spec_init(&s,UMI_UI_PLACEMENT_LEFT,7U,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignNavigationSpecTransferCases(&s) != 0) return 1;
return s.item_count==7U?0:2;}
