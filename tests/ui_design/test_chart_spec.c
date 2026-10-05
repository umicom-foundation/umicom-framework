/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_chart_spec.c
 *
 * PURPOSE:
 *   Verify reusable chart interaction and series semantics.
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
#include "umicom/ui/design/chart_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/chart_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignChartSpecTransferEqual(const UmiDesignChartSpec *a, const UmiDesignChartSpec *b)
{
    return a->kind == b->kind &&
        a->series_count == b->series_count &&
        a->legend == b->legend &&
        a->crosshair == b->crosshair &&
        a->zoom == b->zoom &&
        a->pan == b->pan;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignChartSpecTransferTails(UmiDesignChartSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignChartSpecTransferMalformed(const UmiDesignChartSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignChartSpecTransferCases, UmiDesignChartSpec,
    umi_design_chart_spec_archive_encode, umi_design_chart_spec_archive_decode,
    UmiDesignChartSpecTransferEqual, UmiDesignChartSpecTransferTails, UmiDesignChartSpecTransferMalformed)

int main(void){UmiDesignChartSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_chart_spec_init(&s,UMI_DESIGN_CHART_CANDLESTICK,1U,0,1,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignChartSpecTransferCases(&s) != 0) return 1;
return s.crosshair&&s.zoom?0:2;}
