/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_kpi_spec.c
 *
 * PURPOSE:
 *   Verify the semantic kpi spec contract.
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
#include "umicom/ui/design/kpi_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/kpi_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignKpiSpecTransferEqual(const UmiDesignKpiSpec *a, const UmiDesignKpiSpec *b)
{
    return strcmp(a->label, b->label) == 0 &&
        a->value == b->value &&
        a->change == b->change &&
        a->role == b->role &&
        a->percentage == b->percentage;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignKpiSpecTransferTails(UmiDesignKpiSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignKpiSpecTransferMalformed(const UmiDesignKpiSpec *sample)
{
    (void)sample;
    {
        UmiDesignKpiSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_kpi_spec_valid(&invalid)) ||
            umi_design_kpi_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignKpiSpecTransferCases, UmiDesignKpiSpec,
    umi_design_kpi_spec_archive_encode, umi_design_kpi_spec_archive_decode,
    UmiDesignKpiSpecTransferEqual, UmiDesignKpiSpecTransferTails, UmiDesignKpiSpecTransferMalformed)

int main(void){UmiDesignKpiSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_kpi_spec_init(&s,"Latency",12.0,-2.0,UMI_DESIGN_ROLE_SUCCESS,0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignKpiSpecTransferCases(&s) != 0) return 1;
return s.value==12.0?0:2;}
