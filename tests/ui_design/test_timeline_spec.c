/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_timeline_spec.c
 *
 * PURPOSE:
 *   Verify the semantic timeline spec contract.
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
#include "umicom/ui/design/timeline_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/timeline_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTimelineSpecTransferEqual(const UmiDesignTimelineSpec *a, const UmiDesignTimelineSpec *b)
{
    return a->tracks == b->tracks &&
        a->pixels_per_unit == b->pixels_per_unit &&
        a->snapping == b->snapping &&
        a->zoomable == b->zoomable &&
        a->scrubbable == b->scrubbable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTimelineSpecTransferTails(UmiDesignTimelineSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTimelineSpecTransferMalformed(const UmiDesignTimelineSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTimelineSpecTransferCases, UmiDesignTimelineSpec,
    umi_design_timeline_spec_archive_encode, umi_design_timeline_spec_archive_decode,
    UmiDesignTimelineSpecTransferEqual, UmiDesignTimelineSpecTransferTails, UmiDesignTimelineSpecTransferMalformed)

int main(void){UmiDesignTimelineSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_timeline_spec_init(&s,24U,8.0,1,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTimelineSpecTransferCases(&s) != 0) return 1;
return s.tracks==24U?0:2;}
