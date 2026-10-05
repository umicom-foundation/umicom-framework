/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_tab_spec.c
 *
 * PURPOSE:
 *   Verify the semantic tab spec contract.
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
#include "umicom/ui/design/tab_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/tab_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTabSpecTransferEqual(const UmiDesignTabSpec *a, const UmiDesignTabSpec *b)
{
    return a->closable == b->closable &&
        a->pinnable == b->pinnable &&
        a->dirty == b->dirty &&
        a->attention == b->attention &&
        a->accent_role == b->accent_role;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTabSpecTransferTails(UmiDesignTabSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTabSpecTransferMalformed(const UmiDesignTabSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTabSpecTransferCases, UmiDesignTabSpec,
    umi_design_tab_spec_archive_encode, umi_design_tab_spec_archive_decode,
    UmiDesignTabSpecTransferEqual, UmiDesignTabSpecTransferTails, UmiDesignTabSpecTransferMalformed)

int main(void){UmiDesignTabSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_tab_spec_init(&s,1,1,1,0,UMI_DESIGN_ROLE_ACCENT)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTabSpecTransferCases(&s) != 0) return 1;
return s.dirty?0:2;}
