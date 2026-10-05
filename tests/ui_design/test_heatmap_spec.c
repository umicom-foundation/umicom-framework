/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_heatmap_spec.c
 *
 * PURPOSE:
 *   Verify the semantic heatmap spec contract.
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
#include "umicom/ui/design/heatmap_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/heatmap_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignHeatmapSpecTransferEqual(const UmiDesignHeatmapSpec *a, const UmiDesignHeatmapSpec *b)
{
    return a->rows == b->rows &&
        a->columns == b->columns &&
        a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->show_labels == b->show_labels;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignHeatmapSpecTransferTails(UmiDesignHeatmapSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignHeatmapSpecTransferMalformed(const UmiDesignHeatmapSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignHeatmapSpecTransferCases, UmiDesignHeatmapSpec,
    umi_design_heatmap_spec_archive_encode, umi_design_heatmap_spec_archive_decode,
    UmiDesignHeatmapSpecTransferEqual, UmiDesignHeatmapSpecTransferTails, UmiDesignHeatmapSpecTransferMalformed)

int main(void){UmiDesignHeatmapSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_heatmap_spec_init(&s,10U,12U,-1.0,1.0,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignHeatmapSpecTransferCases(&s) != 0) return 1;
return s.columns==12U?0:2;}
