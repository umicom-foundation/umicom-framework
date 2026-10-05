/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_card_spec.c
 *
 * PURPOSE:
 *   Verify the semantic card spec contract.
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
#include "umicom/ui/design/card_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/card_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignCardSpecTransferEqual(const UmiDesignCardSpec *a, const UmiDesignCardSpec *b)
{
    return a->role == b->role &&
        a->elevation_level == b->elevation_level &&
        a->interactive == b->interactive &&
        a->selected == b->selected;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignCardSpecTransferTails(UmiDesignCardSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignCardSpecTransferMalformed(const UmiDesignCardSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignCardSpecTransferCases, UmiDesignCardSpec,
    umi_design_card_spec_archive_encode, umi_design_card_spec_archive_decode,
    UmiDesignCardSpecTransferEqual, UmiDesignCardSpecTransferTails, UmiDesignCardSpecTransferMalformed)

int main(void){UmiDesignCardSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_card_spec_init(&s,UMI_DESIGN_ROLE_NEUTRAL,2U,1,0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignCardSpecTransferCases(&s) != 0) return 1;
return s.elevation_level==2U?0:2;}
