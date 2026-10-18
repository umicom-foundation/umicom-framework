/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_gauge_spec.c
 *
 * PURPOSE:
 *   Verify the semantic gauge spec contract.
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
#include "umicom/ui/design/gauge_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/gauge_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignGaugeSpecTransferEqual(const UmiDesignGaugeSpec *a, const UmiDesignGaugeSpec *b)
{
    return a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->value == b->value &&
        a->warning_threshold == b->warning_threshold &&
        a->danger_threshold == b->danger_threshold;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignGaugeSpecTransferTails(UmiDesignGaugeSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignGaugeSpecTransferMalformed(const UmiDesignGaugeSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignGaugeSpecTransferCases, UmiDesignGaugeSpec,
    umi_design_gauge_spec_archive_encode, umi_design_gauge_spec_archive_decode,
    UmiDesignGaugeSpecTransferEqual, UmiDesignGaugeSpecTransferTails, UmiDesignGaugeSpecTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDesignGaugeSpecTransferCasesWithWorkspace, UmiDesignGaugeSpec,
    umi_design_gauge_spec_archive_encode, umi_design_gauge_spec_archive_decode_with_workspace, UmiDesignGaugeSpecTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDesignGaugeSpecConditionalArchiveCases, UmiDesignGaugeSpec,
    umi_design_gauge_spec_archive_encode, umi_design_gauge_spec_archive_decode_if_unchanged,
    UmiDesignGaugeSpecTransferEqual, UmiDesignGaugeSpecTransferTails, UmiDesignGaugeSpecTransferMalformed)


int main(void){UmiDesignGaugeSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_gauge_spec_init(&s,0.0,100.0,72.0,80.0,95.0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignGaugeSpecTransferCases(&s) != 0) return 1;
    if (UmiDesignGaugeSpecTransferCasesWithWorkspace(&s) != 0) return 1;
    if (UmiDesignGaugeSpecConditionalArchiveCases(&s) != 0) return 1;
return s.value==72.0?0:2;}
