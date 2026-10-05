/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_inspector_spec.c
 *
 * PURPOSE:
 *   Verify the semantic inspector spec contract.
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
#include "umicom/ui/design/inspector_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/inspector_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignInspectorSpecTransferEqual(const UmiDesignInspectorSpec *a, const UmiDesignInspectorSpec *b)
{
    return a->category_count == b->category_count &&
        a->estimated_properties == b->estimated_properties &&
        a->searchable == b->searchable &&
        a->advanced_toggle == b->advanced_toggle;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignInspectorSpecTransferTails(UmiDesignInspectorSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignInspectorSpecTransferMalformed(const UmiDesignInspectorSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignInspectorSpecTransferCases, UmiDesignInspectorSpec,
    umi_design_inspector_spec_archive_encode, umi_design_inspector_spec_archive_decode,
    UmiDesignInspectorSpecTransferEqual, UmiDesignInspectorSpecTransferTails, UmiDesignInspectorSpecTransferMalformed)

int main(void){UmiDesignInspectorSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_inspector_spec_init(&s,8U,250U,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignInspectorSpecTransferCases(&s) != 0) return 1;
return s.searchable?0:2;}
