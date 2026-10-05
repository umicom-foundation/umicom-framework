/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_table_spec.c
 *
 * PURPOSE:
 *   Verify the semantic table spec contract.
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
#include "umicom/ui/design/table_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/table_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignTableSpecTransferEqual(const UmiDesignTableSpec *a, const UmiDesignTableSpec *b)
{
    return a->columns == b->columns &&
        a->frozen_columns == b->frozen_columns &&
        a->density == b->density &&
        a->virtualised == b->virtualised &&
        a->sortable == b->sortable &&
        a->filterable == b->filterable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignTableSpecTransferTails(UmiDesignTableSpec *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignTableSpecTransferMalformed(const UmiDesignTableSpec *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignTableSpecTransferCases, UmiDesignTableSpec,
    umi_design_table_spec_archive_encode, umi_design_table_spec_archive_decode,
    UmiDesignTableSpecTransferEqual, UmiDesignTableSpecTransferTails, UmiDesignTableSpecTransferMalformed)

int main(void){UmiDesignTableSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_table_spec_init(&s,12U,2U,UMI_DESIGN_DENSITY_COMPACT,1,1,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignTableSpecTransferCases(&s) != 0) return 1;
return s.frozen_columns==2U?0:2;}
