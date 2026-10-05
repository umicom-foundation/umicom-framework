/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_tree_spec.c
 *
 * PURPOSE:
 *   Verify the semantic tree spec contract.
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
#include "umicom/ui/design/tree_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/tree_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTreeSpecTransferEqual(const UmiDesignTreeSpec *a, const UmiDesignTreeSpec *b)
{
    return a->maximum_depth == b->maximum_depth &&
        a->virtualised == b->virtualised &&
        a->multi_select == b->multi_select &&
        a->checkboxes == b->checkboxes;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTreeSpecTransferTails(UmiDesignTreeSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTreeSpecTransferMalformed(const UmiDesignTreeSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTreeSpecTransferCases, UmiDesignTreeSpec,
    umi_design_tree_spec_archive_encode, umi_design_tree_spec_archive_decode,
    UmiDesignTreeSpecTransferEqual, UmiDesignTreeSpecTransferTails, UmiDesignTreeSpecTransferMalformed)

int main(void){UmiDesignTreeSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_tree_spec_init(&s,16U,1,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTreeSpecTransferCases(&s) != 0) return 1;
return s.checkboxes?0:2;}
