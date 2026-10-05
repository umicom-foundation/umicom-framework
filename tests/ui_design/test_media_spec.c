/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_media_spec.c
 *
 * PURPOSE:
 *   Verify media presentation semantics distinguish image and timed media.
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
#include "umicom/ui/design/media_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/media_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignMediaSpecTransferEqual(const UmiDesignMediaSpec *a, const UmiDesignMediaSpec *b)
{
    return a->kind == b->kind &&
        a->controls == b->controls &&
        a->autoplay == b->autoplay &&
        a->loop == b->loop &&
        a->preserve_aspect == b->preserve_aspect;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignMediaSpecTransferTails(UmiDesignMediaSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignMediaSpecTransferMalformed(const UmiDesignMediaSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignMediaSpecTransferCases, UmiDesignMediaSpec,
    umi_design_media_spec_archive_encode, umi_design_media_spec_archive_decode,
    UmiDesignMediaSpecTransferEqual, UmiDesignMediaSpecTransferTails, UmiDesignMediaSpecTransferMalformed)

int main(void){UmiDesignMediaSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_media_spec_init(&s,UMI_DESIGN_MEDIA_VIDEO,1,0,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignMediaSpecTransferCases(&s) != 0) return 1;
return s.loop?0:2;}
